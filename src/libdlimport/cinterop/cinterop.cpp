#include "cinterop.hpp"

void dlimport::push_cinterop(lua_State *L) {
    const luaL_Reg lib[] = {
        {"new_function_binding", cinterop::new_function_binding},
        //{"new_aggregate", cinterop::new_aggregate},
        {"create_struct_info", cinterop::create_struct_info},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    cinterop::push_c_types(L);
    lua_setfield(L, -2, "native_type");
}
