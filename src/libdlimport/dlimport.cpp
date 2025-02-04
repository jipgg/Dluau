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
#include <array>
#include "dlimport.hpp"
#include <shared.hpp>
#include <variant>
using shared::has_permissions;
using common::error_trail;
using std::string, std::string_view;
using std::variant, std::reference_wrapper;
using dlimport::dlmodule;
using dlimport::dlmodule_ref;


static int search_path(lua_State* L) {
    if (auto path = dlimport::search_path(luaL_checkstring(L, 1))) {
        lua_pushstring(L, path->string().c_str());
        return 1;
    }
    return 0;
}
static int load(lua_State* L) {
    auto module = dlimport::load_module(L);
    if (auto err = std::get_if<error_trail>(&module)) {
        luaL_errorL(L, err->formatted().c_str());
    }
    dlmodule& m = std::get<dlmodule_ref>(module);
    dlimport::lua_pushmodule(L, &m);
    return 1;
}
static int require_module(lua_State* L) {
    const string name = luaL_checkstring(L, 1);
    auto result = dlimport::load_module(L);
    if (auto err = std::get_if<error_trail>(&result)) {
        luaL_errorL(L, err->formatted().c_str());
    }
    dlmodule& module = std::get<reference_wrapper<dlmodule>>(result);
    constexpr const char* function_signature = "dlrequire";
    auto proc = dlimport::find_proc_address(module, function_signature);
    if (not proc) luaL_errorL(L, "module '%s' does not export a symbol '%s'.", name.c_str(), function_signature);
    lua_pushcfunction(L, reinterpret_cast<lua_CFunction>(*proc), (name + std::format(" {}", function_signature)).c_str());
    lua_call(L, 0, 1);
    return 1;
}
static int protected_load(lua_State* L) {
    auto module = dlimport::load_module(L);
    if (auto err = std::get_if<error_trail>(&module)) {
        lua_pushnil(L);
        lua_pushstring(L, err->formatted().c_str());
        return 2;
    }
    dlmodule& m = std::get<dlmodule_ref>(module);
    dlimport::lua_pushmodule(L, &m);
    return 1;
}
static int loaded_modules(lua_State* L) {
    lua_newtable(L);
    int i{1};
    for (const auto& [path, module] : dlimport::get_dlmodules()) {
        dlimport::lua_pushmodule(L, module.get());
        lua_rawseti(L, -2, i++);
    }
    return 1;
}
void dluauopen_dlimport(lua_State* L) {
    dlmodule::init(L);
    const luaL_Reg lib[] = {
        {"require", require_module},
        {"load", load},
        {"pload", protected_load},
        {"searchpath", search_path},
        {"getmodules", loaded_modules},
        {nullptr, nullptr}
    };
    luaL_register(L, "dlimport", lib);
}
