#include "cinterop.hpp"
#include <print>
#include <dluau.h>

DLUAUSTD_API auto dlrequire(lua_State *L) -> int {
    const luaL_Reg lib[] = {
        {"struct_info", cinterop::create_struct_info},
        {"bind_function", cinterop::new_function_binding},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    return 1;
}

DLUAUSTD_API auto test_function(long arg) -> int {
    std::println("This is a test function {}", arg);
    return 123;
}
