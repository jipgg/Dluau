#include <Windows.h>
#include <lualib.h>
#include <luacode.h>
#include "luminutils.h"
#include <ranges>
#include <lumin.h>
#include <format>
#include <iostream>

static int run(const std::string& source) {
    lua_State* L = luaL_newstate();
    lua_callbacks(L)->useratom = lumin_useratom;
    luaL_openlibs(L);
    luminopen_dll(L);
    lumin_loadfuncs(L);
    luminU_spawnscript(L, source.c_str());
    return lua_status(L);
}
int lumin_main(std::span<std::string_view> args) {
    for (auto e : args) {
        std::cout << std::format("> {}\n", e);
    }
    if (args[0] == "run") return run(std::string(args[1]));
    return 0;
}
