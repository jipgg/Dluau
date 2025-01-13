#include <Windows.h>
#include <lualib.h>
#include <luacode.h>
#include "luminutils.h"
#include <lumin.h>
#include <iostream>
int lumin_main(std::string_view args) {
    lua_State* L = luaL_newstate();
    lua_callbacks(L)->useratom = lumin_useratom;
    luaL_openlibs(L);
    luminopen_dll(L);
    lumin_loadfuncs(L);
    luminU_spawnscript(L, "test.luau");
    if (lua_status(L) != LUA_OK) std::cerr << "\033[31m" <<lua_tostring(L, -1) << "\033[0m\n"; 
    return lua_status(L);
}
