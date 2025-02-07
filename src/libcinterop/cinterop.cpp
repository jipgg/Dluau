#include "cinterop.hpp"
#include <dluau.h>

void dluauopen_cinterop(lua_State *L) {
    const luaL_Reg structlib[] = {
        {"newinfo", cinterop::create_struct_info},
        {"getfield", cinterop::get_struct_field},
        {"setfield", cinterop::set_struct_field},
        {"newinstance", cinterop::new_struct_instance},
        {nullptr, nullptr}
    };
    const luaL_Reg lib[] = {
        {"bindfunction", cinterop::new_function_binding},
        {"sizeof", cinterop::c_type_sizeof},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    lua_newtable(L);
    luaL_register(L, nullptr, structlib);
    lua_setfield(L, -2, "struct");
    cinterop::push_c_types(L);
    lua_setfield(L, -2, "native");
    lua_setglobal(L, "cinterop");
}
