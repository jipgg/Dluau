#include "cinterop.hpp"
#include <print>
#include <gpm_api.h>
#include <dluau.h>

DLUAUSTD_API auto dlrequire(lua_State *L) -> int {
    const luaL_Reg lib[] = {
        {"StructInfo", cinterop::create_struct_info},
        {"bind_function", cinterop::new_function_binding},
        {"sizeof", cinterop::c_type_sizeof},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    cinterop::register_c_types(L);
    return 1;
}

DLUAUSTD_API auto test_function(long arg) -> int {
    std::println("This is a test function {}", arg);
    return 123;
}
