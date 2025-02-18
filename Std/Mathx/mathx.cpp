#include "gpm_api.h"
#include "mathx.hpp"


template <class T, class Vec = T::Vector>
static auto cross_namecall(lua_State* L, Vec& self) -> int {
    T::init(L) = blaze::cross(self, T::check(L, 2));
    return 1;
}

DLUAUSTD_API auto dlrequire(lua_State* L) -> int {
    V3Type::register_this_namecall(L, "cross", cross_namecall<V3Type>);
    //V3fType::register_this_namecall(L, "cross", cross_namecall<V3fType>);
    V3i16Type::register_this_namecall(L, "cross", cross_namecall<V3i16Type>);
    const luaL_Reg ctors[] = {
        {"Vector3", V3Type::lua_ctor},
        {"Vector3i16", V3i16Type::lua_ctor},
        {"Vector2", V2Type::lua_ctor},
        {"Vector2i16", V2i16Type::lua_ctor},
        {"Vector4", V4Type::lua_ctor},
        {"Vector4i16", V4i16Type::lua_ctor},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, ctors);
    M3Type::register_type(L, "Matrix3");
    M3i16Type::register_type(L, "Matrix3i16");
    M4Type::register_type(L, "Matrix4");
    M4i16Type::register_type(L, "Matrix4i16");
    return 1;
}
