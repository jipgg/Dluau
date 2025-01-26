#include "dluau.h"
#include <lualib.h>
#include <string>
#include <lualib.h>
#include <luacode.h>
#include "luacode.h"
#include <iostream>
#include <cassert>
#include <shared.hpp>

static bool codegen = true;
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
    lua_insert(L, -2); // put before error message
    return 2;          // return nil plus error message
}

int lua_collectgarbage(lua_State* L) {
    const char* option = luaL_optstring(L, 1, "collect");
    if (strcmp(option, "collect") == 0) {
        lua_gc(L, LUA_GCCOLLECT, 0);
        return 0;
    }
    if (strcmp(option, "count") == 0) {
        int c = lua_gc(L, LUA_GCCOUNT, 0);
        lua_pushnumber(L, c);
        return 1;
    }
    luaL_error(L, "collectgarbage must be called with 'count' or 'collect'");
}

static int lua_scan(lua_State* L) {
    std::string in;
    std::cin >> in;
    lua_pushstring(L, in.c_str());
    return 1;
}

void dluau_loadfuncs(lua_State *L) {
    const luaL_Reg global_functions[] = {
        {"loadstring", lua_loadstring},
        //{"require", lua_require},
        {"collectgarbage", lua_collectgarbage},
        {"scan", lua_scan},
        {"require", shared::dluau_require},
        {nullptr, nullptr}
    };
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, NULL, global_functions);
    lua_pop(L, 1);
}
