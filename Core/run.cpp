#include <dluau.hpp>
#include <format>
#include "luacode.h"
#include <lua.h>
#include <common.hpp>
#include <format>
#include <lualib.h>
#include <boost/container/flat_map.hpp>
#include <nlohmann/json.hpp>
#include <print>
#include <iostream>
using nlohmann::json;
using std::string_view, std::string;
namespace vws = std::views;
using std::println, std::cerr;

static lua_CompileOptions copts{.debugLevel = 1};
lua_CompileOptions* dluau::compile_options{&copts};

static auto lua_require(lua_State* L) -> int {
    return dluau_require(L, luaL_checkstring(L, 1));
}
static auto lua_lazyrequire(lua_State* L) -> int {
    return dluau_lazyrequire(L, luaL_checkstring(L, 1));
}
static auto lua_loadstring(lua_State* L) -> int {
    size_t l = 0;
    const char* s = luaL_checklstring(L, 1, &l);
    const char* chunkname = luaL_optstring(L, 2, s);
    lua_setsafeenv(L, LUA_ENVIRONINDEX, false);
    size_t outsize;
    char* bc = luau_compile(s, l, dluau::compile_options, &outsize);
    string bytecode(s, outsize);
    std::free(bc);
    if (luau_load(L, chunkname, bytecode.data(), bytecode.size(), 0) == 0)
        return 1;
    lua_pushnil(L);
    lua_insert(L, -2);
    return 2;
}
static auto lua_collectgarbage(lua_State* L) -> int {
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
    dluau::error(L, "collectgarbage must be called with 'count' or 'collect'");
}
void dluau_registerglobals(lua_State* L) {
    const luaL_Reg global_functions[] = {
        {"loadstring", lua_loadstring},
        {"collectgarbage", lua_collectgarbage},
        {"require", lua_require},
        {"lazyrequire", lua_lazyrequire},
        {nullptr, nullptr}
    };
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, NULL, global_functions);
    lua_pop(L, 1);
}
void dluau_openlibs(lua_State* L) {
    luaL_openlibs(L);
    dluauopen_print(L);
    dluauopen_scan(L);
    dluauopen_dlimport(L);
    dluauopen_task(L);
    dluauopen_os(L);
}
auto dluau_newuserdatatag() -> int {
    static int curr_type_tag = 1;
    return curr_type_tag++; 
}
auto dluau_newlightuserdatatag() -> int {
    static int curr_type_tag = 1;
    return curr_type_tag++; 
}
auto dluau_newstate() -> lua_State* {
    lua_State* L = luaL_newstate();
    lua_callbacks(L)->useratom = dluau::default_useratom;
    dluau_registerglobals(L);
    return L;
}
auto dluau_run(const dluau_RunOptions* opts) -> int {
    dluau::compile_options->debugLevel = 3;
    dluau::compile_options->optimizationLevel = opts->optimization_level;
    if (opts->args) dluau::args = opts->args;
    std::unique_ptr<lua_State, decltype(&lua_close)> state{dluau_newstate(), lua_close}; 
    lua_State* L = state.get();
    dluau_openlibs(L);
    if (opts->global_functions) {
        lua_pushvalue(L, LUA_GLOBALSINDEX);
        luaL_register(L, nullptr, opts->global_functions);
        lua_pop(L, 1);
    }
    luaL_sandbox(L);
    constexpr const char* errfmt = "\033[31m{}\033[0m";
    if (opts->scripts == nullptr) {
        println(cerr, errfmt, "no sources given");
        return -1;
    }
    for (auto sr : vws::split(string_view(opts->scripts), dluau::arg_separator)) {
        string_view script{sr.data(), sr.size()};
        if (auto result = dluau::run_file(L, script); !result) {
            println(cerr, errfmt, result.error());
            return -1;
        }
    }
    while (dluau::tasks_in_progress()) {
        if (auto result = dluau::task_step(L); !result) {
            println(cerr, errfmt, result.error());
            return -1;
        }
    }
    std::cout << "\033[0m";
    return 0;
}

namespace dluau {
auto has_permissions(lua_State* L) -> bool {
    lua_Debug ar;
    if (not lua_getinfo(L, 1, "s", &ar)) return false;
    if (ar.source[0] == '@' or ar.source[0] == '=') return true;
    return false;
}
auto run_file(lua_State* L, string_view script_path) -> expected<void, string> {
    auto r = ::dluau::load_file(L, script_path);
    if (not r) return unexpected(r.error());
    auto* co = *r;
    int status = lua_resume(co, L, 0);
    if (status != LUA_OK and status != LUA_YIELD) {
        return unexpected(luaL_checkstring(co, -1));
    }
    return expected<void, string>{};
}
}
