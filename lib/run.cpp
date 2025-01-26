#include "dluau.h"
#include <fstream>
#include <optional>
#include <format>
#include "luacode.h"
#include <lua.h>
#include <filesystem>
#include <common.hpp>
#include <format>
#include <lualib.h>
#include <boost/container/flat_map.hpp>
#include <nlohmann/json.hpp>
#include <variant>
#include <shared.hpp>
namespace fs = std::filesystem;
static lua_CompileOptions copts{.debugLevel = 1};
lua_CompileOptions* dluau::compile_options{&copts};
using std::string, std::string_view;
using std::stringstream, std::ifstream;
using common::error_trail;
using boost::container::flat_map;
using nlohmann::json, fs::path;
using std::optional, std::variant;
using std::nullopt, std::format;

static bool config_file_initialized{false};
static flat_map<string, string> aliases;
static flat_map<string, int> modules;
static flat_map<lua_State*, string> script_paths;

static optional<path> find_config_file() {
    const auto config_file_names = std::to_array<string_view>({".luaurc", ".dluaurc"});
    constexpr int search_depth{5};
    path root = fs::current_path();
    auto exists = [&cfn = config_file_names, &root](path& out) -> bool {
        for (string_view name : cfn) {
            const path potential_path = root / name;
            if (fs::exists(potential_path)) {
                out = potential_path;
                return true;
            } 
        }
        return false;
    };
    int depth{};
    path potential_path;
    while (not exists(potential_path) and depth++ < search_depth) root = root.parent_path();
    if (potential_path.empty()) return nullopt;
    return potential_path;
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
    auto source = common::read_file(*found_config);
    if (not source) return error_trail(format("couldn't read source '{}.'", found_config->string()));
    json parsed = json::parse(*source);
    if (not parsed.contains("aliases")) return nullopt;
    for (auto [key, val] : parsed["aliases"].items()) {
        if (not val.is_string()) {
            return error_trail(format("value in [{}] must be a string '{}'.", key, nlohmann::to_string(val)));
        }
        auto p = common::sanitize_path(string(val));
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
    auto path = common::sanitize_path(found_source->string());
    if (not path) return error_trail("failed to sanitize path"); 
    return *path;
}
//api
int dluau_require(lua_State* L, const char* name) {
    if (not config_file_initialized) {
        config_file_initialized = true;
        if (auto err = load_aliases()) {
            luaL_errorL(L, err->formatted().c_str());
        }
    }
    const path script_root{path(script_paths.at(L)).parent_path()};
    auto result = resolve_path(name, script_root);
    if (auto* err = std::get_if<error_trail>(&result)) {
        luaL_errorL(L, err->formatted().c_str());
    }
    const string file_path{std::move(std::get<string>(result))};
    if (modules.contains(file_path)) {
        lua_getref(L, modules[file_path]);
        return 1;
    }
    auto source = common::read_file(file_path).value_or("");
    if (source.empty()) [[unlikely]] luaL_errorL(L, "couldn't read source '%s'", file_path.c_str());
    lua_State* M = lua_newthread(lua_mainthread(L));
    luaL_sandboxthread(M);
    script_paths.emplace(M, file_path);
    dluau::precompile(source);
    size_t bc_len;
    char* bc_arr = luau_compile(source.data(), source.size(), dluau::compile_options, &bc_len);
    common::raii free_after([&bc_arr]{std::free(bc_arr);});
    const string chunkname = '@' + common::make_path_pretty(file_path);
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
void dluau_openlibs(lua_State *L) {
    luaL_openlibs(L);
    dluau_loadfuncs(L);
    dluau::push_print(L);
    lua_setglobal(L, "print");
    dluau::push_scan(L);
    lua_setglobal(L, "scan");
    dluauopen_dlimport(L);
}
int dluau_newuserdatatag() {
    static int curr_type_tag = 1;
    return curr_type_tag++; 
}
int dluau_newlightuserdatatag() {
    static int curr_type_tag = 1;
    return curr_type_tag++; 
}
lua_State* dluau_newstate() {
    lua_State* L = luaL_newstate();
    lua_callbacks(L)->useratom = dluau::default_useratom;
    dluau_openlibs(L);
    dluau::push_metadatatable(L);
    lua_setglobal(L, "meta");
    return L;
}
int dluau_run(const dluau_Run_options* opts) {
    dluau::compile_options->debugLevel = opts->debug_level;
    dluau::compile_options->optimizationLevel = opts->optimization_level;
    if (opts->args) dluau::args = opts->args;
    std::unique_ptr<lua_State, decltype(&lua_close)> state{dluau_newstate(), lua_close}; 
    lua_State* L = state.get();
    if (opts->global_functions) {
        lua_pushvalue(L, LUA_GLOBALSINDEX);
        luaL_register(L, nullptr, opts->global_functions);
        lua_pop(L, 1);
    }
    luaL_sandbox(L);
    constexpr const char* errfmt = "\033[31m{}\033[0m\n";
    if (opts->scripts == nullptr) {
        std::cerr << format(errfmt, "no sources given.");
        return -1;
    }
    string errmsg;
    using std::views::split;
    for (auto sr : split(string_view(opts->scripts), dluau::arg_separator)) {
        string_view script{sr.data(), sr.size()};
        if (auto err = dluau::run_file(L, script)) {
            errmsg = err->formatted();
            break;
        }
        std::cout << "\033[0m";
    }
    if (not errmsg.empty()) {
        std::cerr << format(errfmt, errmsg);
        return -1;
    }
    return 0;
}
//cpp
namespace dluau {
int require(lua_State* L) {
    return dluau_require(L, luaL_checkstring(L, 1));
}
bool has_permissions(lua_State* L) {
    lua_Debug ar;
    if (not lua_getinfo(L, 1, "s", &ar)) return false;
    if (ar.source[0] == '@' or ar.source[0] == '=') return true;
    return false;
}
const flat_map<lua_State*, string>& get_script_paths() {
    return script_paths;
}
optional<lua_State*> load_file(lua_State* L, string_view path) {
    string script_path{path};
    optional<string> source = common::read_file(script_path);
    if (not source) return nullopt;
    auto identifier = common::make_path_pretty(common::sanitize_path(script_path).value_or(script_path));
    identifier = "=" + identifier;
    size_t outsize;
    dluau::precompile(*source);
    char* bc = luau_compile(
        source->data(), source->size(),
        compile_options, &outsize
    );
    string bytecode{bc, outsize};
    std::free(bc);
    lua_State* script_thread = lua_newthread(L);
    script_paths.emplace(script_thread, fs::absolute(path).string());
    const int load_status = luau_load(script_thread, identifier.c_str(), bytecode.data(), bytecode.size(), 0);
    if (load_status == LUA_OK) {
        luaL_sandboxthread(script_thread);
        return script_thread;
    }
    return nullopt;
}
optional<error_trail> run_file(lua_State* L, string_view script_path) {
    auto r = load_file(L, script_path);
    if (not r) return error_trail(format("failed to load script '{}'", script_path));
    int status = lua_resume(*r, L, 0);
    if (status != LUA_OK and status != LUA_YIELD) {
        return error_trail{luaL_checkstring(*r, -1)};
    }
    return nullopt;
}
}
