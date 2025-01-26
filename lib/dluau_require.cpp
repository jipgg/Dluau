#include "shared.hpp"
#include <dluau.h>
#include <boost/container/flat_map.hpp>
#include <nlohmann/json.hpp>
#include <common.hpp>
#include <variant>
#include <filesystem>
namespace fs = std::filesystem;
namespace cm = common;
namespace bc = boost::container;
using nlohmann::json, fs::path;
using std::string, std::string_view;
using std::optional, std::variant, cm::error_trail;
using std::nullopt, std::format, bc::flat_map;

static bool initialized{false};
static flat_map<string, string> aliases;
static flat_map<string, int> modules;

static optional<path> find_config_file() {
    constexpr int search_depth{5};
    path root = fs::current_path();
    int depth{};
    while (not fs::exists(root / ".luaurc") and depth++ < search_depth) root = root.parent_path();
    if (not fs::exists(root / ".luaurc")) return nullopt; 
    return root / ".luaurc";
}

static bool has_alias(const string& str) {
    return str[0] == '@';
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
    std::cout << format("SUB: {}\n", str);
    return nullopt;
}

static optional<path> find_source(path p, const path& root) {
    if (p.is_relative()) p = root / p;
    if (not fs::exists(p)) {
        if (not p.has_extension()) {
            p.replace_extension(".luau");
            if (fs::exists(p)) return p;
        }
    } else if (fs::is_directory(p)) {
        p /= "init.luau";
        if (fs::exists(p)) return p; 
    } else return p;
    return nullopt;
}

static optional<error_trail> load_aliases() {
    auto found_config = find_config_file();
    if (not found_config) return nullopt;
    auto source = cm::read_file(*found_config);
    if (not source) return error_trail(format("couldn't read source '{}.'", found_config->string()));
    json parsed = json::parse(*source);
    if (not parsed.contains("aliases")) return nullopt;
    for (auto [key, val] : parsed["aliases"].items()) {
        if (not val.is_string()) {
            return error_trail(format("value in [{}] must be a string '{}'.", key, nlohmann::to_string(val)));
        }
        auto p = cm::sanitize_path(string(val));
        if (not p) return error_trail(format("failed to sanitize path '{}'", string(val)));
        std::cout << format("NEW ALIAS: {} -> {}\n", key, *p);
        aliases.emplace(key, std::move(*p));
    }
    return nullopt;
}

static variant<string, error_trail> resolve_path(string name, const path& root) {
    if (has_alias(name)) {
        if (auto err = substitute_alias(name)) return err->propagate();
    }
    auto found_source = find_source(name, root);
    if (not found_source) return error_trail("couldnt find source path");
    auto path = cm::sanitize_path(found_source->string());
    if (not path) return error_trail("failed to sanitize path"); 
    return *path;
}

int shared::dluau_require(lua_State* L) {
    if (not initialized) {
        initialized = true;
        if (auto err = load_aliases()) {
            luaL_errorL(L, err->formatted().c_str());
        }
    }
    const path script_root{path(shared::script_paths.at(L)).parent_path()};
    auto result = resolve_path(luaL_checkstring(L, 1), script_root);
    if (auto* err = std::get_if<error_trail>(&result)) {
        luaL_errorL(L, err->formatted().c_str());
    }
    const string file_path{std::move(std::get<string>(result))};
    if (modules.contains(file_path)) {
        lua_getref(L, modules[file_path]);
        return 1;
    }
    auto source = cm::read_file(file_path).value_or("");
    if (source.empty()) [[unlikely]] luaL_errorL(L, "couldn't read source '%s'", file_path.c_str());

    lua_State* M = lua_newthread(lua_mainthread(L));
    luaL_sandboxthread(M);
    shared::script_paths.emplace(M, file_path);
    shared::process_precompiled_features(source);
    size_t bc_len;
    char* bc_arr = luau_compile(source.data(), source.size(), shared::compile_options, &bc_len);
    cm::raii free_after([&bc_arr]{std::free(bc_arr);});
    const string chunkname = '@' + cm::make_path_pretty(file_path);
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
    modules.emplace(file_path, lua_ref(L, -1));
    return 1;
}
