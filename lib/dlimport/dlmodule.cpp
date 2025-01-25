#include "lib.hpp"
#include <format>
#include <common/error_trail.hpp>
constexpr int unintialized{-1};
static int lua_cfunction_stringatom{unintialized};
static int create_binding_stringatom{unintialized};
using std::string_view, std::string;
using common::error_trail;

static int index(lua_State* L) {
    dlmodule* module = util::lua_tomodule(L, 1);
    const string_view key = luaL_checkstring(L, 2);
    if (key == "absolute_path") {
        lua_pushstring(L, module->path.c_str());
        return 1;
    } else if (key == "name") {
        lua_pushstring(L, module->name.c_str());
        return 1;
    }
    luaL_argerrorL(L, 2, "index was null");
}

static int lua_cfunction(lua_State* L) {
    using namespace std::string_literals;
    const string proc_key = "lua_"s + luaL_checkstring(L, 2);
    auto opt = util::find_proc_address(*util::lua_tomodule(L, 1), proc_key);
    if (not opt) luaL_errorL(L, "lua_CFunction was not found ");
    const auto fmt = std::format("CFunction: {}", proc_key);
    lua_pushcfunction(L, reinterpret_cast<lua_CFunction>(*opt), fmt.c_str());
    return 1;
}
static int namecall(lua_State* L) {
    dlmodule& module = *util::lua_tomodule(L, 1);
    int atom;
    lua_namecallatom(L, &atom);
    if (atom == lua_cfunction_stringatom) return lua_cfunction(L);
    else if(atom == create_binding_stringatom) return dlmodule::create_binding(L);
    luaL_errorL(L, error_trail{std::format("invalid namecall '{}'", atom) }.formatted().c_str());
}

void dlmodule::init(lua_State* L) {
    if (luaL_newmetatable(L, dlmodule::tname)) {
        lua_cfunction_stringatom = dluau_stringatom(L, "lua_pushcfunction");
        create_binding_stringatom = dluau_stringatom(L, "create_c_binding");
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
