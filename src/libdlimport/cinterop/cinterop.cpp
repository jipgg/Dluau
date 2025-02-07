#include "cinterop.hpp"

void dlimport::push_cinterop(lua_State *L) {
    const luaL_Reg structlib[] = {
        {"newinfo", cinterop::create_struct_info},
        {"getfield", cinterop::get_struct_field},
        {"setfield", cinterop::set_struct_field},
        {nullptr, nullptr}
    };
    const luaL_Reg lib[] = {
        {"bindfunction", cinterop::new_function_binding},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    lua_newtable(L);
    luaL_register(L, nullptr, structlib);
    lua_setfield(L, -2, "struct");
    cinterop::push_c_types(L);
    lua_setfield(L, -2, "native");
}
