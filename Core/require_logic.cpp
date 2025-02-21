#include <dluau.hpp>
#include <common.hpp>
#include <boost/container/flat_map.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <optional>
namespace fs = std::filesystem;
namespace rngs = std::ranges;
using nlohmann::json;
using std::span;
using dluau::def_file_exts;
using boost::container::flat_map;
using std::string, std::regex, std::pair;
using std::expected, std::unexpected;
using fs::path, std::string_view;
using std::format, std::optional;

static flat_map<string, string> aliases;
static flat_map<string, int> luamodules;
static flat_map<lua_State*, string> script_paths;
static bool config_file_initialized{false};

static auto find_config_file(path base = fs::current_path(), int search_depth = 5) -> expected<path, string> {
    const auto config_file_names = std::to_array<>({
        ".luaurc", ".dluaurc",
        ".dluaurc.json", ".luaurc.json",
        "luaurc.json", "dluaurc.json",
    });
    auto exists = [&cfn = config_file_names, &base](path& out) -> bool {
        for (string_view name : cfn) {
            const path potential_path = base / name;
            if (fs::exists(potential_path)) {
                out = potential_path;
                return true;
            } 
        }
        return false;
    };
    int depth{};
    path potential_path;
    while (not exists(potential_path) and depth++ < search_depth) base = base.parent_path();
    if (potential_path.empty()) return std::unexpected("could not find config file");
    return potential_path;
}
static auto has_alias(const string& str) -> bool {
    return str[0] == '@';
}
static auto load_aliases(const path& base = fs::current_path(), int search_depth = 5) -> expected<void, string> {
    auto found_config = find_config_file(base, search_depth);
    if (not found_config) return expected<void, string>();

    auto source = common::read_file(*found_config);
    if (not source) return unexpected(std::format("couldn't read source '{}.'", found_config.value().string()));
    json parsed = json::parse(*source);
    if (not parsed.contains("aliases")) return expected<void, string>();
    for (auto [key, val] : parsed["aliases"].items()) {
        if (not val.is_string()) {
            return unexpected(format("value in [{}] must be a string '{}'.", key, nlohmann::to_string(val)));
        }
        path path{string(val)};
        auto with_user_folder = common::substitute_user_folder(path);
        if (not with_user_folder) return unexpected(with_user_folder.error());
        else path = *with_user_folder;
        auto normalized = common::normalize_path(path);
        aliases.emplace(key, std::move(normalized.string()));
    }
    return expected<void, string>();
}
static auto substitute_alias(string& str) -> expected<void, string> {
    static const regex alias_regex{R"(\@[A-Za-z][A-Za-z0-9_-]*)"};
    std::smatch sm;
    if (not std::regex_search(str, sm, alias_regex)) {
        return unexpected(format("failed match allias regex {}", str));
    }
    const string alias = sm.str().substr(1);
    if (not aliases.contains(alias)) {
        return unexpected(format("alias '{}' does not exist.", alias));
    }
    str.replace(sm.position(), sm.length(), aliases[alias]);
    return expected<void, string>{};
}
static auto lazyrequire_handler(lua_State* L) -> int {
    try {
        const string name = lua_tostring(L, lua_upvalueindex(1));
        dluau::require(L, name.c_str());
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
auto dluau_lazyrequire(lua_State* L, const char* name) -> int {
    lua_newtable(L);
    lua_newtable(L);
    lua_pushstring(L, "__index");
    auto resolved = dluau::resolve_require_path(L, name);
    if (!resolved) dluau::error(L, resolved.error());
    dluau::push(L, *resolved);
    lua_pushcclosure(L, lazyrequire_handler, "lazyrequire_handler", 1);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    return 1;
}
auto dluau::require(lua_State* L, string_view name) -> int {
    auto resolved = ::dluau::resolve_require_path(L, string(name));
    if (not resolved) ::dluau::error(L, resolved.error());
    const string file_path{std::move(*resolved)};
    if (luamodules.contains(file_path)) {
        lua_getref(L, luamodules[file_path]);
        return 1;
    }
    const auto& preprocessed = dluau::get_preprocessed_modules();
    std::string source;
    if (preprocessed.contains(file_path)) [[likely]] {
        source = preprocessed.at(file_path).source;
    } else {
        source = common::read_file(file_path).value_or("");
        if (source.empty()) [[unlikely]] luaL_errorL(L, "couldn't read source '%s'", file_path.c_str());
        dluau::precompile(source, dluau::get_precompiled_library_values(file_path));
    }
    lua_State* M = lua_newthread(lua_mainthread(L));
    luaL_sandboxthread(M);
    script_paths.emplace(M, file_path);
    size_t bc_len;
    char* bc_arr = luau_compile(source.data(), source.size(), ::dluau::compile_options, &bc_len);
    common::Raii free_after([&bc_arr]{std::free(bc_arr);});
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
static auto find_source(path p, const path& base, span<const string> file_exts = def_file_exts) -> optional<path> {
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
    return std::nullopt;
}
namespace dluau {
auto resolve_require_path(const fs::path& base, string name, span<const string> file_exts) -> expected<string, string> {
    if (not config_file_initialized) {
        config_file_initialized = true;
        if (auto loaded = load_aliases(); !loaded) return loaded.error();
    }
    const fs::path path_name{name};
    const bool maybe_preprocessed = path_name.is_absolute() and fs::is_regular_file(path_name) and path_name.extension() == ".luau";
    if (maybe_preprocessed) {
        return path_name.string();
    }
    path script_path{base};
    if (name[0] == '@') {
        if (auto success = substitute_alias(name); !success) return success.error();
    } else if (name[0] == '~') {
        auto with_user = common::substitute_user_folder(name);
        if (not with_user) return unexpected(with_user.error());
        name = with_user.value().string();
    }
    auto found_source = find_source(name, script_path, file_exts);
    if (not found_source) {
        return unexpected(format("couldn't find source for '{}'", name));
    }
    return common::normalize_path(*found_source).string();
}
auto resolve_require_path(lua_State* L, string name, span<const string> file_exts) -> expected<string, string> {
    path script_path{name};
    if (not script_path.is_absolute()) {
        if (script_paths.contains(L)) script_path = path(script_paths.at(L)).parent_path();
        else unexpected("runtime relative require is only allowed from a script thread");
    } 
    return resolve_require_path(script_path, name, file_exts);
}
auto resolve_path(string name, const path& base, span<const string> file_exts) -> expected<string, string> {
    if (has_alias(name)) {
        if (auto ok = substitute_alias(name); !ok) return ok.error();
    } else if (name[0] == '~') {
        auto with_user = common::substitute_user_folder(name);
        if (not with_user) return unexpected(with_user.error());
        name = (*with_user).string();
    }
    auto found_source = find_source(name, base, file_exts);
    if (not found_source) return unexpected(format("couldn't find source for '{}'", name));
    return common::normalize_path(*found_source).string();
}
auto get_aliases() -> const flat_map<string, string>& {
    return aliases;
}
auto get_script_paths() -> flat_map<lua_State*, string>& {
    return script_paths;
}
}
