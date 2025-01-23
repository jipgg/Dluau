#include "lib.hpp"
#include <format>
static int lua_cfunction_stringatom{};
static int bind_function_stringatom{};

static int index(lua_State* L) {
    Dlmodule* module = util::lua_tomodule(L, 1);
    const std::string_view key = luaL_checkstring(L, 2);
    if (key == "path") {
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
    const std::string proc_key = "lua_"s + luaL_checkstring(L, 2);
    auto opt = util::find_proc_address(*util::lua_tomodule(L, 1), proc_key);
    if (not opt) luaL_errorL(L, "lua_CFunction was not found ");
    const auto fmt = std::format("CFunction: {}", proc_key);
    lua_pushcfunction(L, reinterpret_cast<lua_CFunction>(*opt), fmt.c_str());
    return 1;
}
static int namecall(lua_State* L) {
    Dlmodule& module = *util::lua_tomodule(L, 1);
    int atom;
    lua_namecallatom(L, &atom);
    if (atom == lua_cfunction_stringatom) return lua_cfunction(L);
    else if(atom == bind_function_stringatom) return Dlmodule::create_binding(L);
    luaL_errorL(L, "invalid");
}

void Dlmodule::init(lua_State* L) {
    if (luaL_newmetatable(L, Dlmodule::tname)) {
        lua_cfunction_stringatom = luauxt_stringatom(L, "lua_cfunction");
        bind_function_stringatom = luauxt_stringatom(L, "bind");
        lua_setlightuserdataname(L, Dlmodule::tag, Dlmodule::tname);
        const luaL_Reg meta[] = {
            {"__index", index},
            {"__namecall", namecall},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
    }
    lua_pop(L, 1);
}
