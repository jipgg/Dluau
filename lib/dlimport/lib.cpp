#include "luauxt.h"
#include <unordered_map>
#include <string>
#include <luacode.h>
#include <Luau/Common.h>
#include <memory>
#include <cstdint>
#include <optional>
#include <ranges>
#include <algorithm>
#include <Windows.h>
#include <cassert>
#include <boost/container/flat_map.hpp>
#include <core.hpp>
#include <format>
#include <filesystem>
#include <dyncall.h>
#include <iostream>
#include "lib.hpp"
#include <format>
namespace fs = std::filesystem;
namespace rn = std::ranges;

static int findpath(lua_State* L) {
    if (auto path = util::find_module_path(luaL_checkstring(L, 1))) {
        lua_pushstring(L, path->c_str());
        return 1;
    }
    return 0;
}
static int load(lua_State* L) {
    Dlmodule* module = util::init_or_find_module(luaL_checkstring(L, 1));
    if (not module) luaL_argerrorL(L, 1, "couldn't find dll");
    util::lua_pushmodule(L, module);
    return 1;
}
static int cfunction(lua_State* L) {
    const char* proc_key = luaL_checkstring(L, 2);
    auto opt = util::find_proc_address(*util::lua_tomodule(L, 1), proc_key);
    if (not opt) luaL_errorL(L, "function was not found ");
    const auto fmt = std::format("{}", proc_key);
    lua_pushcfunction(L, reinterpret_cast<lua_CFunction>(*opt), fmt.c_str());
    return 1;
}
static int find(lua_State* L) {
    auto module = util::init_or_find_module(luaL_checkstring(L, 1));
    if (not module) return 0;
    util::lua_pushmodule(L, module);
    return 1;
}
int luauxtload_dlimport(lua_State* L) {
    Dlmodule::init(L);
    const luaL_Reg lib[] = {
        {"load", load},
        {"find", find},
        {"cfunction", cfunction},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    return 1;
}
void luauxtopen_dlimport(lua_State* L) {
    luauxtload_dlimport(L);
    lua_setglobal(L, "dlimport");
}
