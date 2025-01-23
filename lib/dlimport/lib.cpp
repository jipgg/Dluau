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

static int search_for(lua_State* L) {
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
static int try_load(lua_State* L) {
    auto module = util::init_or_find_module(luaL_checkstring(L, 1));
    if (not module) return 0;
    util::lua_pushmodule(L, module);
    return 1;
}
int luauxtload_dlimport(lua_State* L) {
    Dlmodule::init(L);
    const luaL_Reg lib[] = {
        {"load", load},
        {"try_load", try_load},
        {"search_for", search_for},
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
