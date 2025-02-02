#include "dlimport.hpp"
#include <format>
#include <common/error_trail.hpp>
#include <common.hpp>
#include <filesystem>
constexpr int unintialized{-1};
static int importfunction_sa{unintialized};
static int rawcbinding_sa{unintialized};
using std::string_view, std::string;
using common::error_trail;
using dlimport::dlmodule;

static int index(lua_State* L) {
    dlmodule* module = dlimport::lua_tomodule(L, 1);
    const string_view key = luaL_checkstring(L, 2);
    if (key == "path") {
        lua_pushstring(L, module->path.string().c_str());
        return 1;
    } else if (key == "name") {
        lua_pushstring(L, module->name.c_str());
        return 1;
    }
    luaL_argerrorL(L, 2, "index was null");
}

static int importfunction(lua_State* L) {
    const string proc_key = string("dlexport_") + luaL_checkstring(L, 2);
    auto opt = dlimport::find_proc_address(*dlimport::lua_tomodule(L, 1), proc_key);
    if (not opt) luaL_errorL(L, "lua_CFunction '%s' was not found ", proc_key.c_str());
    const auto fmt = std::format("dlimported:{}", proc_key);
    lua_pushcfunction(L, reinterpret_cast<lua_CFunction>(*opt), fmt.c_str());
    return 1;
}
static int namecall(lua_State* L) {
    dlmodule& module = *dlimport::lua_tomodule(L, 1);
    int atom;
    lua_namecallatom(L, &atom);
    if (atom == importfunction_sa) return importfunction(L);
    else if(atom == rawcbinding_sa) return dlmodule::create_binding(L);
    luaL_errorL(L, error_trail{std::format("invalid namecall '{}'", atom) }.formatted().c_str());
}

void dlmodule::init(lua_State* L) {
    if (luaL_newmetatable(L, dlmodule::tname)) {
        importfunction_sa = dluau_stringatom(L, "importfunction");
        rawcbinding_sa = dluau_stringatom(L, "rawcbinding");
        lua_setlightuserdataname(L, dlmodule::tag, dlmodule::tname);
        const luaL_Reg meta[] = {
            {"__index", index},
            {"__namecall", namecall},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
    }
    lua_pop(L, 1);
}
