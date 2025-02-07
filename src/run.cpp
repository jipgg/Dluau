#include "dluau.h"
#include <fstream>
#include <optional>
#include <format>
#include "luacode.h"
#include <lua.h>
#include <common.hpp>
#include <format>
#include <lualib.h>
#include <boost/container/flat_map.hpp>
#include <nlohmann/json.hpp>
#include <variant>
#include "shared.hpp"
using std::string, std::string_view;
using std::stringstream, std::ifstream;
using common::error_trail;
using nlohmann::json;
using std::optional;
using std::nullopt, std::format;
using std::get, std::get_if;

static lua_CompileOptions copts{.debugLevel = 1};
lua_CompileOptions* shared::compile_options{&copts};

static int lua_require(lua_State* L) {
    return dluau_require(L, luaL_checkstring(L, 1));
}
static int lua_loadstring(lua_State* L) {
    size_t l = 0;
    const char* s = luaL_checklstring(L, 1, &l);
    const char* chunkname = luaL_optstring(L, 2, s);
    lua_setsafeenv(L, LUA_ENVIRONINDEX, false);
    size_t outsize;
    char* bc = luau_compile(s, l, shared::compile_options, &outsize);
    std::string bytecode(s, outsize);
    std::free(bc);
    if (luau_load(L, chunkname, bytecode.data(), bytecode.size(), 0) == 0)
        return 1;
    lua_pushnil(L);
    lua_insert(L, -2);
    return 2;
}
int lua_collectgarbage(lua_State* L) {
    string_view option = luaL_optstring(L, 1, "collect");
    if (option == "collect") {
        lua_gc(L, LUA_GCCOLLECT, 0);
        return 0;
    }
    if (option == "count") {
        int c = lua_gc(L, LUA_GCCOUNT, 0);
        lua_pushnumber(L, c);
        return 1;
    }
    luaL_error(L, "collectgarbage must be called with 'count' or 'collect'");
}
void dluau_registerglobals(lua_State *L) {
    const luaL_Reg global_functions[] = {
        {"loadstring", lua_loadstring},
        {"collectgarbage", lua_collectgarbage},
        {"require", lua_require},
        {nullptr, nullptr}
    };
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, NULL, global_functions);
    lua_pop(L, 1);
}
void dluau_openlibs(lua_State *L) {
    luaL_openlibs(L);
    dluauopen_print(L);
    dluauopen_scan(L);
    dluauopen_dlimport(L);
    dluauopen_task(L);
    dluauopen_os(L);
    dluauopen_cinterop(L);
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
    lua_callbacks(L)->useratom = shared::default_useratom;
    dluau_registerglobals(L);
    return L;
}
int dluau_run(const dluau_runoptions* opts) {
    shared::compile_options->debugLevel = 3;
    shared::compile_options->optimizationLevel = opts->optimization_level;
    if (opts->args) shared::args = opts->args;
    std::unique_ptr<lua_State, decltype(&lua_close)> state{dluau_newstate(), lua_close}; 
    lua_State* L = state.get();
    dluau_openlibs(L);
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
    using std::views::split;
    for (auto sr : split(string_view(opts->scripts), shared::arg_separator)) {
        string_view script{sr.data(), sr.size()};
        if (auto err = shared::run_file(L, script)) {
            std::cerr << format(errfmt, err->formatted());
            return -1;
        }
    }
    while (shared::tasks_in_progress()) {
        if (auto err = shared::task_step(L)) {
            std::cerr << format(errfmt, err->formatted());
            return -1;
        }
    }
    std::cout << "\033[0m";
    return 0;
}

namespace shared {
bool has_permissions(lua_State* L) {
    lua_Debug ar;
    if (not lua_getinfo(L, 1, "s", &ar)) return false;
    if (ar.source[0] == '@' or ar.source[0] == '=') return true;
    return false;
}
optional<error_trail> run_file(lua_State* L, string_view script_path) {
    auto r = load_file(L, script_path);
    if (auto* err = get_if<error_trail>(&r)) return err->propagate();

    auto* co = get<lua_State*>(r);
    int status = lua_resume(co, L, 0);
    if (status != LUA_OK and status != LUA_YIELD) {
        return error_trail{luaL_checkstring(co, -1)};
    }
    return nullopt;
}
}
