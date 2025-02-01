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
#include "local.hpp"
#include <shared.hpp>
namespace filesystem = std::filesystem;
using shared::has_permissions;
using common::error_trail;
using std::string, std::string_view;

constexpr const char* err_loading_perm{"dl loading is not allowed in current script environment."};

static int search_path(lua_State* L) {
    if (auto path = util::search_path(luaL_checkstring(L, 1))) {
        lua_pushstring(L, path->c_str());
        return 1;
    }
    return 0;
}
static const auto dl_file_extensions = std::to_array<string>({".so", ".dll", ".dylib"});
static dlmodule& load_module(lua_State* L) {
    if (not has_permissions(L)) luaL_errorL(L, err_loading_perm);
    const string name = luaL_checkstring(L, 1);
    auto resolved = shared::resolve_require_path(L, name, dl_file_extensions);
    if (auto err = std::get_if<error_trail>(&resolved)) {
        luaL_errorL(L, "couldn't resolve path for '%s'.", name.c_str());
    }
    auto p = std::get<string>(resolved);
    std::cout << "P IS " << p << '\n';
    return util::init_module(p);
}
static int load(lua_State* L) {
    dlmodule& module = load_module(L);
    util::lua_pushmodule(L, &module);
    return 1;
}
static int require_module(lua_State* L) {
    const string name = luaL_checkstring(L, 1);
    dlmodule& module = load_module(L);
    constexpr const char* function_signature = "dlrequire";
    auto proc = util::find_proc_address(module, function_signature);
    if (not proc) luaL_errorL(L, "module '%s' does not export a symbol '%s'.", name.c_str(), function_signature);
    lua_pushcfunction(L, reinterpret_cast<lua_CFunction>(*proc), (name + std::format(" {}", function_signature)).c_str());
    lua_call(L, 0, 1);
    return 1;
}
static int protected_load(lua_State* L) {
    if (not has_permissions(L)) {
        lua_pushnil(L);
        lua_pushstring(L, err_loading_perm);
        return 2;
    }
    auto module = util::init_or_find_module(luaL_checkstring(L, 1));
    if (not module) {
        lua_pushnil(L);
        luaL_errorL(L, "couldn't find module '%s'.", luaL_checkstring(L, 1));
        return 2;
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
        {"require", require_module},
        {"load", load},
        {"pload", protected_load},
        {"searchpath", search_path},
        {"getmodules", loaded_modules},
        {nullptr, nullptr}
    };
    luaL_register(L, "dlimport", lib);
}
