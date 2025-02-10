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
#include <dyncall.h>
#include <array>
#include "dlimport.hpp"
#include <variant>
using namespace dluau::type_aliases;
using dluau::has_permissions;
using common::error_trail;
using dlimport::Dlmodule;


static int search_path(Lstate L) {
    if (auto path = dlimport::search_path(luaL_checkstring(L, 1))) {
        lua_pushstring(L, path->string().c_str());
        return 1;
    }
    return 0;
}
static int load(Lstate L) {
    auto module = dlimport::load_module(L);
    if (not module) dluau::error(L, module.error());
    dlimport::lua_pushmodule(L, &module->get());
    return 1;
}
static int require_module(Lstate L) {
    const String name = luaL_checkstring(L, 1);
    auto result = dlimport::load_module(L);
    if (!result) dluau::error(L, result.error());
    Dlmodule& module = *result;
    constexpr const char* function_signature = "dlrequire";
    auto proc = dlimport::find_proc_address(module, function_signature);
    if (not proc) luaL_errorL(L, "module '%s' does not export a symbol '%s'.", name.c_str(), function_signature);
    lua_pushcfunction(L, reinterpret_cast<lua_CFunction>(*proc), (name + std::format(" {}", function_signature)).c_str());
    lua_call(L, 0, 1);
    return 1;
}
static int protected_load(Lstate L) {
    auto module = dlimport::load_module(L);
    if (!module) {
        lua_pushnil(L);
        dluau::push(L, module.error());
        return 2;
    }
    Dlmodule& m = *module;
    dlimport::lua_pushmodule(L, &m);
    return 1;
}
static int loaded_modules(Lstate L) {
    lua_newtable(L);
    int i{1};
    for (const auto& [path, module] : dlimport::get_dlmodules()) {
        dlimport::lua_pushmodule(L, module.get());
        lua_rawseti(L, -2, i++);
    }
    return 1;
}
void dluauopen_dlimport(Lstate L) {
    Dlmodule::init(L);
    lua_newtable(L);
    const Lreg lib[] = {
        {"require", require_module},
        {"load", load},
        {"pload", protected_load},
        {"searchpath", search_path},
        {"getmodules", loaded_modules},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, lib);
    lua_setglobal(L, "dlimport");
}
