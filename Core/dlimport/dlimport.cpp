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
#include "dlimport.hpp"
#include <variant>
using dluau::has_permissions;
using std::string;

static auto search_path(lua_State* L) -> int {
    if (auto path = dlimport::search_path(luaL_checkstring(L, 1))) {
        lua_pushstring(L, path->string().c_str());
        return 1;
    }
    return 0;
}
static auto load(lua_State* L) -> int {
    auto module = dlimport::load_module(L);
    if (not module) dluau::error(L, module.error());
    dlimport::lua_pushmodule(L, &module->get());
    return 1;
}
static auto require_module(lua_State* L) -> int {
    const std::string name = luaL_checkstring(L, 1);
    auto result = dlimport::load_module(L);
    if (!result) dluau::error(L, result.error());
    dluau_Dlmodule& module = *result;
    constexpr const char* function_signature = "dlrequire";
    auto proc = dlimport::find_proc_address(module, function_signature);
    if (not proc) luaL_errorL(L, "module '%s' does not export a symbol '%s'.", name.c_str(), function_signature);
    lua_pushcfunction(L, reinterpret_cast<lua_CFunction>(*proc), (name + std::format(" {}", function_signature)).c_str());
    lua_call(L, 0, 1);
    return 1;
}
static auto protected_load(lua_State* L) -> int {
    auto module = dlimport::load_module(L);
    if (!module) {
        lua_pushnil(L);
        dluau::push(L, module.error());
        return 2;
    }
    dluau_Dlmodule& m = *module;
    dlimport::lua_pushmodule(L, &m);
    return 1;
}
static auto loaded_modules(lua_State* L) -> int {
    lua_newtable(L);
    int i{1};
    for (const auto& [path, module] : dlimport::get_dlmodules()) {
        dlimport::lua_pushmodule(L, module.get());
        lua_rawseti(L, -2, i++);
    }
    return 1;
}
auto dluau_todlmodule(lua_State* L, int idx) -> dluau_Dlmodule* {
    return dlimport::lua_tomodule(L, idx);
}
void dluau_pushdlmodule(lua_State* L, dluau_Dlmodule* dlm) {
    dlimport::lua_pushmodule(L, dlm);
}
auto dluau_dlmodulefind(dluau_Dlmodule* dlm, const char* symbol) -> uintptr_t {
    return dlimport::find_proc_address(*dlm, symbol).value_or(0);
}
auto dluau_loaddlmodule(lua_State* L, const char* require_path) -> dluau_Dlmodule* {
    auto res = dlimport::load_module(L, require_path);
    if (!res) return nullptr;
    return &(res->get());
}
void dluauopen_dlimport(lua_State* L) {
    dluau_Dlmodule::init(L);
    lua_newtable(L);
    const luaL_Reg lib[] = {
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
