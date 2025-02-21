#include "dluau.h"
#include <string>
#include <luacode.h>
#include <Luau/Common.h>
#include <optional>
#include <ranges>
#include <Windows.h>
#include <cassert>
#include <boost/container/flat_map.hpp>
#include <dluau.hpp>
#include <filesystem>
#include <array>
#include <variant>
using dluau::has_permissions;
using std::string;

static auto search_path(lua_State* L) -> int {
    if (auto path = dluau::search_path(luaL_checkstring(L, 1))) {
        lua_pushstring(L, path->string().c_str());
        return 1;
    }
    return 0;
}
static auto load(lua_State* L) -> int {
    auto module = dluau::dlload(L);
    if (not module) dluau::error(L, module.error());
    dluau::push_dlmodule(L, &module->get());
    return 1;
}
static auto require_module(lua_State* L) -> int {
    const std::string name = luaL_checkstring(L, 1);
    auto result = dluau::dlload(L);
    if (!result) dluau::error(L, result.error());
    dluau_Dlmodule& module = *result;
    constexpr const char* function_signature = "dlrequire";
    auto proc = dluau::find_dlmodule_proc_address(module, function_signature);
    if (not proc) luaL_errorL(L, "module '%s' does not export a symbol '%s'.", name.c_str(), function_signature);
    lua_pushcfunction(L, reinterpret_cast<lua_CFunction>(*proc), (name + std::format(" {}", function_signature)).c_str());
    lua_call(L, 0, 1);
    return 1;
}
static auto protected_load(lua_State* L) -> int {
    auto module = dluau::dlload(L);
    if (!module) {
        lua_pushnil(L);
        dluau::push(L, module.error());
        return 2;
    }
    dluau_Dlmodule& m = *module;
    dluau::push_dlmodule(L, &m);
    return 1;
}
static auto loaded_modules(lua_State* L) -> int {
    lua_newtable(L);
    int i{1};
    for (const auto& [path, module] : dluau::get_dlmodules()) {
        dluau::push_dlmodule(L, module.get());
        lua_rawseti(L, -2, i++);
    }
    return 1;
}
void dluau::open_dlimport_library(lua_State* L) {
    dluau_Dlmodule::init(L);
    lua_newtable(L);
    const luaL_Reg lib[] = {
        {"dlrequire", require_module},
        {"load", load},
        {"try_load", protected_load},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, lib);
    lua_setglobal(L, "dlimport");
}
