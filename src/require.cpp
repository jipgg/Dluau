#include "shared.hpp"
#include <common.hpp>
#include <boost/container/flat_map.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
namespace filesystem = std::filesystem;
using std::optional, std::nullopt;
using boost::container::flat_map;
using std::string, std::string_view;
using std::span;
using nlohmann::json;
using std::pair, std::regex;
using fspath = filesystem::path;
using common::error_trail;
using shared::default_file_extensions;

static flat_map<string, string> aliases;
static flat_map<string, int> luamodules;
static flat_map<lua_State*, string> script_paths;
static bool config_file_initialized{false};

static auto get_precompiled_library_values(const string& p) {
    auto as_string_literal = [](const string& str) {
        return format("(\"{}\")", str);
    };
    const auto arr = std::to_array<std::pair<regex, string>>({
        {std::regex(R"(\bscript.directory\b)"), as_string_literal(fspath(p).parent_path().string())},
        {regex(R"(\bscript.path\b)"), as_string_literal(p)},
        {regex(R"(\bscript.name\b)"), as_string_literal(fspath(p).stem().string())},
    });
    return arr;
}

static optional<fspath> find_config_file(fspath root = filesystem::current_path(), int search_depth = 5) {
    const auto config_file_names = std::to_array<string_view>({
        ".luaurc", ".dluaurc",
        ".dluaurc.json", ".luaurc.json",
        "luaurc.json", "dluaurc.json",
    });
    auto exists = [&cfn = config_file_names, &root](fspath& out) -> bool {
        for (string_view name : cfn) {
            const fspath potential_path = root / name;
            if (filesystem::exists(potential_path)) {
                out = potential_path;
                return true;
            } 
        }
        return false;
    };
    int depth{};
    fspath potential_path;
    while (not exists(potential_path) and depth++ < search_depth) root = root.parent_path();
    if (potential_path.empty()) return nullopt;
    return potential_path;
}
static bool has_alias(const string& str) {
    return str[0] == '@';
}
static optional<error_trail> load_aliases(const fspath& root = filesystem::current_path(), int search_depth = 5) {
    auto found_config = find_config_file(root, search_depth);
    if (not found_config) return nullopt;
    auto source = common::read_file(*found_config);
    if (not source) return error_trail(format("couldn't read source '{}.'", found_config->string()));
    json parsed = json::parse(*source);
    if (not parsed.contains("aliases")) return nullopt;
    for (auto [key, val] : parsed["aliases"].items()) {
        if (not val.is_string()) {
            return error_trail(format("value in [{}] must be a string '{}'.", key, nlohmann::to_string(val)));
        }
        string valstr{val};
        if (valstr.at(0) == '$') {
            auto opt = common::substitute_environment_variable(valstr);
            if (not opt) return error_trail(format("failed to find env path '{}'", valstr));
            valstr = *opt;
        } else if (valstr.at(0) == '~') {
            auto opt = common::substitute_user_folder(valstr);
            if (not opt) return error_trail("failed to get user folder");
            valstr = *opt;
        }
        auto path = common::sanitize_path(valstr, root);
        aliases.emplace(key, std::move(path));
    }
    return nullopt;
}
static optional<error_trail> substitute_alias(string& str) {
    static const std::regex alias_regex{R"(\@[A-Za-z][A-Za-z0-9_-]*)"};
    std::smatch sm;
    if (not std::regex_search(str, sm, alias_regex)) {
        return error_trail(format("failed match allias regex {}", str));
    }
    const string alias = sm.str().substr(1);
    if (not aliases.contains(alias)) {
        return error_trail(format("alias '{}' does not exist.", alias));
    }
    str.replace(sm.position(), sm.length(), aliases[alias]);
    return nullopt;
}

int dluau_require(lua_State* L, const char* name) {
    if (not config_file_initialized) {
        config_file_initialized = true;
        if (auto err = load_aliases()) {
            luaL_errorL(L, err->message().c_str());
        }
    }
    if (not script_paths.contains(L)) luaL_errorL(L, "require is only allowed from a script thread");
    const fspath script_root{fspath(script_paths.at(L)).parent_path()};
    auto result = shared::resolve_path(name, script_root);
    if (auto* err = std::get_if<error_trail>(&result)) {
        luaL_errorL(L, err->message().c_str());
    }
    const string file_path{std::move(std::get<string>(result))};
    if (luamodules.contains(file_path)) {
        lua_getref(L, luamodules[file_path]);
        return 1;
    }
    auto source = common::read_file(file_path).value_or("");
    if (source.empty()) [[unlikely]] luaL_errorL(L, "couldn't read source '%s'", file_path.c_str());
    lua_State* M = lua_newthread(lua_mainthread(L));
    luaL_sandboxthread(M);
    script_paths.emplace(M, file_path);
    const auto pretty_path = common::make_path_pretty(file_path);
    shared::precompile(source, get_precompiled_library_values(pretty_path));
    size_t bc_len;
    char* bc_arr = luau_compile(source.data(), source.size(), shared::compile_options, &bc_len);
    common::raii free_after([&bc_arr]{std::free(bc_arr);});
    const string chunkname = '@' + pretty_path;
    int status{-1};
    if (luau_load(M, chunkname.c_str(), bc_arr, bc_len, 0) == LUA_OK) {
        status = lua_resume(M, L, 0);
        const int top = lua_gettop(M);
        switch (status) {
            case LUA_OK:
                if (top != 1) {
                    lua_pushstring(M, "module must return 1 value.");
                    status = -1;
                }
            break;
            case LUA_YIELD:
                lua_pushstring(M, "module can not yield.");
            break;
        }
    }
    lua_xmove(M, L, 1);
    if (status != LUA_OK) {
        luaL_errorL(L, lua_tostring(L, -1));
    }
    lua_pushvalue(L, -1);
    luamodules.emplace(file_path, lua_ref(L, -1));
    return 1;
}
static optional<fspath> find_source(fspath p, const fspath& base, span<const string> file_exts = default_file_extensions) {
    if (p.is_relative()) p = base / p;
    if (not filesystem::exists(p)) {
        if (not p.has_extension()) {
            for (const string& ext : file_exts) {
                p.replace_extension(ext);
                if (filesystem::exists(p)) return p;
            }
        }
    } else if (filesystem::is_directory(p)) {
        for (const string& ext : file_exts) {
            p /= std::format("init{}", ext);
            if (filesystem::exists(p)) return p; 
            p = p.parent_path();
        }
    } else return p;
    return nullopt;
}
namespace shared {
variant<lua_State*, error_trail> load_file(lua_State* L, string_view path) {
    string script_path{path};
    optional<string> source = common::read_file(script_path);
    if (not source) return error_trail(format("couldn't read source '{}'.", script_path));
    auto identifier = common::make_path_pretty(common::sanitize_path(script_path));
    shared::precompile(*source, get_precompiled_library_values(identifier));
    identifier = "=" + identifier;
    size_t outsize;
    char* bc = luau_compile(
        source->data(), source->size(),
        compile_options, &outsize
    );
    string bytecode{bc, outsize};
    std::free(bc);
    lua_State* script_thread = lua_newthread(L);
    script_paths.emplace(script_thread, filesystem::absolute(path).string());
    const int load_status = luau_load(script_thread, identifier.c_str(), bytecode.data(), bytecode.size(), 0);
    if (load_status == LUA_OK) {
        luaL_sandboxthread(script_thread);
        return script_thread;
    }
    return error_trail(format("failed to load '{}'\nreason: {}\nsource: {}", script_path, lua_tostring(script_thread, -1), *source));
}

variant<string, error_trail> resolve_require_path(lua_State* L, string name, span<const string> file_exts) {
    if (not config_file_initialized) {
        config_file_initialized = true;
        if (auto err = load_aliases()) err->propagate();
    }
    if (not script_paths.contains(L)) luaL_errorL(L, "require is only allowed from a script thread");
    const fspath script_path{fspath(script_paths.at(L)).parent_path()};
    auto result = shared::resolve_path(name, script_path, file_exts);
    if (auto* err = std::get_if<error_trail>(&result)) {
        luaL_errorL(L, err->message().c_str());
    }
    return std::get<string>(result);
}
variant<string, error_trail> resolve_path(string name, const path& base, span<const string> file_exts) {
    if (has_alias(name)) {
        if (auto err = substitute_alias(name)) return err->propagate();
    } else if (name[0] == '~') {
        auto with_user = common::substitute_user_folder(name);
        if (not with_user) return error_trail(std::format("couldn't resolve user profile for '{}'.", name));
        name = *with_user;
    }
    auto found_source = find_source(name, base, file_exts);
    if (not found_source) return error_trail("couldnt find source path");
    return common::sanitize_path(found_source->string());
}
const flat_map<string, string>& get_aliases() {
    return aliases;
}
const flat_map<lua_State*, string>& get_script_paths() {
    return script_paths;
}
}
