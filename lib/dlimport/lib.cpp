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

constexpr const char* err_loading_perm{"dl loading is not allowed in current script environment."};
constexpr const char* err_not_found{"dl not found."};

static bool has_loading_permissions(lua_State* L) {
    lua_Debug ar;
    if (not lua_getinfo(L, 1, "s", &ar)) return false;
    if (ar.source[0] == '@' or ar.source[0] == '=') return true;
    return false;
}

static int search_for(lua_State* L) {
    if (auto path = util::find_module_path(luaL_checkstring(L, 1))) {
        lua_pushstring(L, path->c_str());
        return 1;
    }
    return 0;
}
static int load(lua_State* L) {
    if (not has_loading_permissions(L)) luaL_errorL(L, err_loading_perm);
    Dlmodule* module = util::init_or_find_module(luaL_checkstring(L, 1));
    if (not module) luaL_argerrorL(L, 1, err_not_found);
    util::lua_pushmodule(L, module);
    return 1;
}
static int try_load(lua_State* L) {
    if (not has_loading_permissions(L)) {
        lua_pushstring(L, err_loading_perm);
        return 1;
    }
    auto module = util::init_or_find_module(luaL_checkstring(L, 1));
    if (not module) {
        lua_pushstring(L, err_not_found);
        return 1;
    }
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
