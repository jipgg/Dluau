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
using namespace dluau::type_aliases;
using Json = nlohmann::json;

static lua_CompileOptions copts{.debugLevel = 1};
lua_CompileOptions* dluau::compile_options{&copts};

static int lua_require(Lstate L) {
    return dluau_require(L, luaL_checkstring(L, 1));
}
static int lua_lazyrequire(Lstate L) {
    return dluau_lazyrequire(L, luaL_checkstring(L, 1));
}
static int lua_loadstring(Lstate L) {
    size_t l = 0;
    const char* s = luaL_checklstring(L, 1, &l);
    const char* chunkname = luaL_optstring(L, 2, s);
    lua_setsafeenv(L, LUA_ENVIRONINDEX, false);
    size_t outsize;
    char* bc = luau_compile(s, l, dluau::compile_options, &outsize);
    String bytecode(s, outsize);
    std::free(bc);
    if (luau_load(L, chunkname, bytecode.data(), bytecode.size(), 0) == 0)
        return 1;
    lua_pushnil(L);
    lua_insert(L, -2);
    return 2;
}
static int lua_collectgarbage(Lstate L) {
    Strview option = luaL_optstring(L, 1, "collect");
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
void dluau_registerglobals(Lstate L) {
    const Lreg global_functions[] = {
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
void dluau_openlibs(Lstate L) {
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
Lstate dluau_newstate() {
    Lstate L = luaL_newstate();
    lua_callbacks(L)->useratom = dluau::default_useratom;
    dluau_registerglobals(L);
    return L;
}
int dluau_run(const dluau_runoptions* opts) {
    dluau::compile_options->debugLevel = 3;
    dluau::compile_options->optimizationLevel = opts->optimization_level;
    if (opts->args) dluau::args = opts->args;
    Unique<lua_State, decltype(&lua_close)> state{dluau_newstate(), lua_close}; 
    Lstate L = state.get();
    dluau_openlibs(L);
    if (opts->global_functions) {
        lua_pushvalue(L, LUA_GLOBALSINDEX);
        luaL_register(L, nullptr, opts->global_functions);
        lua_pop(L, 1);
    }
    luaL_sandbox(L);
    constexpr const char* errfmt = "\033[31m{}\033[0m";
    if (opts->scripts == nullptr) {
        std::println(std::cerr, errfmt, "no sources given");
        return -1;
    }
    for (auto sr : views::split(Strview(opts->scripts), dluau::arg_separator)) {
        Strview script{sr.data(), sr.size()};
        if (auto result = dluau::run_file(L, script); !result) {
            std::println(std::cerr, errfmt, result.error());
            return -1;
        }
    }
    while (dluau::tasks_in_progress()) {
        if (auto result = dluau::task_step(L); !result) {
            std::println(std::cerr, errfmt, result.error());
            return -1;
        }
    }
    std::cout << "\033[0m";
    return 0;
}

namespace dluau {
bool has_permissions(Lstate L) {
    lua_Debug ar;
    if (not lua_getinfo(L, 1, "s", &ar)) return false;
    if (ar.source[0] == '@' or ar.source[0] == '=') return true;
    return false;
}
Expected<void> run_file(Lstate L, Strview script_path) {
    auto r = ::dluau::load_file(L, script_path);
    if (not r) return Unexpected(r.error());
    auto* co = *r;
    int status = lua_resume(co, L, 0);
    if (status != LUA_OK and status != LUA_YIELD) {
        return Unexpected(luaL_checkstring(co, -1));
    }
    return Expected<void>();
}
}
