#include "shared.hpp"
#include <common.hpp>
#include <boost/container/flat_map.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
namespace fs = std::filesystem;
namespace ranges = std::ranges;
using std::optional, std::nullopt;
using boost::container::flat_map;
using std::string, std::string_view;
using std::span, std::variant;
using nlohmann::json;
using std::pair, std::regex;
using std::format;
using common::error_trail;
using shared::default_file_extensions;

static flat_map<string, string> aliases;
static flat_map<string, int> luamodules;
static flat_map<lua_State*, string> script_paths;
static bool config_file_initialized{false};

static auto get_precompiled_library_values(const fs::path& p) {
    auto as_string_literal = [](const fs::path& path) {
        auto str = path.string();
        ranges::replace(str, '\\', '/');
        return format("(\"{}\")", str);
    };
    const auto arr = std::to_array<std::pair<regex, string>>({
        {std::regex(R"(\bscript.directory\b)"), as_string_literal(p.parent_path())},
        {regex(R"(\bscript.path\b)"), as_string_literal(p)},
        {regex(R"(\bscript.name\b)"), as_string_literal(fs::path(p).stem())},
    });
    return arr;
}

static variant<fs::path, error_trail> find_config_file(fs::path base = fs::current_path(), int search_depth = 5) {
    const auto config_file_names = std::to_array<string_view>({
        ".luaurc", ".dluaurc",
        ".dluaurc.json", ".luaurc.json",
        "luaurc.json", "dluaurc.json",
    });
    auto exists = [&cfn = config_file_names, &base](fs::path& out) -> bool {
        for (string_view name : cfn) {
            const fs::path potential_path = base / name;
            if (fs::exists(potential_path)) {
                out = potential_path;
                return true;
            } 
        }
        return false;
    };
    int depth{};
    fs::path potential_path;
    while (not exists(potential_path) and depth++ < search_depth) base = base.parent_path();
    if (potential_path.empty()) return error_trail("could not find config file");
    return potential_path;
}
static bool has_alias(const string& str) {
    return str[0] == '@';
}
static optional<error_trail> load_aliases(const fs::path& base = fs::current_path(), int search_depth = 5) {
    auto found_config = find_config_file(base, search_depth);
    if (auto err = get_if<error_trail>(&found_config)) return nullopt;
    auto source = common::read_file(get<fs::path>(found_config));
    if (not source) return error_trail(format("couldn't read source '{}.'", get<fs::path>(found_config).string()));
    json parsed = json::parse(*source);
    if (not parsed.contains("aliases")) return nullopt;
    for (auto [key, val] : parsed["aliases"].items()) {
        if (not val.is_string()) {
            return error_trail(format("value in [{}] must be a string '{}'.", key, nlohmann::to_string(val)));
        }
        fs::path path{string(val)};
        auto with_user_folder = common::substitute_user_folder(path);
        if (auto err = get_if<error_trail>(&with_user_folder)) return err->propagate();
        else path = get<fs::path>(with_user_folder);
        auto normalized = common::normalize_path(path);
        aliases.emplace(key, std::move(normalized.string()));
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
static int lazyrequire_handler(lua_State* L) {
    try {
        const string name = lua_tostring(L, lua_upvalueindex(1));
        dluau_require(L, name.c_str());
        lua_getmetatable(L, 1);
        lua_pushvalue(L, -2);
        lua_setfield(L, -2, "__index");
        lua_pop(L, 1);
        if (lua_istable(L, -1)) {
            lua_pushvalue(L, 2);
            lua_gettable(L, -2);
            lua_remove(L, -2);
        }
        return 1;
    } catch(std::exception& e) {
        luaL_errorL(L, e.what());
    }
}
int dluau_lazyrequire(lua_State* L, const char* name) {
    lua_newtable(L);
    lua_newtable(L);
    lua_pushstring(L, "__index");
    auto result = shared::resolve_require_path(L, name);
    if (auto* err = get_if<error_trail>(&result)) {
        luaL_errorL(L, err->formatted().c_str());
    }
    lua_pushstring(L, std::get<string>(result).c_str());
    lua_pushcclosure(L, lazyrequire_handler, "lazyrequire_handler", 1);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    return 1;
}
int dluau_require(lua_State* L, const char* name) {
    auto result = shared::resolve_require_path(L, name);
    if (auto* err = get_if<error_trail>(&result)) {
        luaL_errorL(L, err->formatted().c_str());
    }
    const string file_path{std::move(get<string>(result))};
    if (luamodules.contains(file_path)) {
        lua_getref(L, luamodules[file_path]);
        return 1;
    }
    auto source = common::read_file(file_path).value_or("");
    if (source.empty()) [[unlikely]] luaL_errorL(L, "couldn't read source '%s'", file_path.c_str());
    lua_State* M = lua_newthread(lua_mainthread(L));
    luaL_sandboxthread(M);
    script_paths.emplace(M, file_path);
    shared::precompile(source, get_precompiled_library_values(file_path));
    size_t bc_len;
    char* bc_arr = luau_compile(source.data(), source.size(), shared::compile_options, &bc_len);
    common::raii free_after([&bc_arr]{std::free(bc_arr);});
    string chunkname = file_path;
    chunkname = '@' + chunkname;
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
static optional<fs::path> find_source(fs::path p, const fs::path& base, span<const string> file_exts = default_file_extensions) {
    if (p.is_relative()) p = base / p;
    if (not fs::exists(p)) {
        if (not p.has_extension()) {
            for (const string& ext : file_exts) {
                p.replace_extension(ext);
                if (fs::exists(p)) return p;
            }
        }
    } else if (fs::is_directory(p)) {
        for (const string& ext : file_exts) {
            p /= std::format("init{}", ext);
            if (fs::exists(p)) return p; 
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
    auto normalized = common::normalize_path(script_path);
    shared::precompile(*source, get_precompiled_library_values(normalized.string()));
    string identifier{fs::relative(script_path).string()};
    ranges::replace(identifier, '\\', '/');
    identifier = '=' + identifier;
    size_t outsize;
    char* bc = luau_compile(
        source->data(), source->size(),
        compile_options, &outsize
    );
    string bytecode{bc, outsize};
    std::free(bc);
    lua_State* script_thread = lua_newthread(L);
    script_paths.emplace(script_thread, normalized.string());
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
    if (not script_paths.contains(L)) return error_trail("require is only allowed from a script thread");
    const fs::path script_path{fs::path(script_paths.at(L)).parent_path()};
    if (name[0] == '@') {
        if (auto err = substitute_alias(name)) return err->propagate();
    } else if (name[0] == '~') {
        auto with_user = common::substitute_user_folder(name);
        if (auto err = get_if<error_trail>(&with_user)) return err->propagate();
        name = get<fs::path>(with_user).string();
    }
    auto found_source = find_source(name, script_path, file_exts);
    if (not found_source) return error_trail(format("couldn't find source for '{}'", name));
    return common::normalize_path(*found_source).string();
}
variant<string, error_trail> resolve_path(string name, const path& base, span<const string> file_exts) {
    if (has_alias(name)) {
        if (auto err = substitute_alias(name)) return err->propagate();
    } else if (name[0] == '~') {
        auto with_user = common::substitute_user_folder(name);
        if (auto err = get_if<error_trail>(&with_user)) return err->propagate();
        name = get<fs::path>(with_user).string();
    }
    auto found_source = find_source(name, base, file_exts);
    if (not found_source) return error_trail(format("couldn't find source for '{}'", name));
    return common::normalize_path(*found_source).string();
}
const flat_map<string, string>& get_aliases() {
    return aliases;
}
const flat_map<lua_State*, string>& get_script_paths() {
    return script_paths;
}
}
