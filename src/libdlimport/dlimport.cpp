#include "dluau.h"
#include <string>
#include <luacode.h>
#include <Luau/Common.h>
#include <optional>
#include <ranges>
#include <Windows.h>
#include <cassert>
#include <boost/container/flat_map.hpp>
#include <shared.hpp>
#include <filesystem>
#include <dyncall.h>
#include "local.hpp"
#include <shared.hpp>
namespace fs = std::filesystem;
using shared::has_permissions;

constexpr const char* err_loading_perm{"dl loading is not allowed in current script environment."};
constexpr const char* err_not_found{"dl not found."};

static int search_path(lua_State* L) {
    if (auto path = util::search_path(luaL_checkstring(L, 1))) {
        lua_pushstring(L, path->c_str());
        return 1;
    }
    return 0;
}
static int load(lua_State* L) {
    if (not has_permissions(L)) luaL_errorL(L, err_loading_perm);
    dlmodule* module = util::init_or_find_module(luaL_checkstring(L, 1));
    if (not module) luaL_argerrorL(L, 1, err_not_found);
    util::lua_pushmodule(L, module);
    return 1;
}
static int try_load(lua_State* L) {
    if (not has_permissions(L)) {
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
static int loaded_modules(lua_State* L) {
    lua_newtable(L);
    int i{1};
    for (const auto& [path, module] : glob::loaded) {
        util::lua_pushmodule(L, module.get());
        lua_rawseti(L, -2, i++);
    }
    return 1;
}
void dluauopen_dlimport(lua_State* L) {
    dlmodule::init(L);
    const luaL_Reg lib[] = {
        {"load", load},
        {"try_load", try_load},
        {"search_path", search_path},
        {"loaded_modules", loaded_modules},
        {nullptr, nullptr}
    };
    luaL_register(L, "dlimport", lib);
}
