#include <dluau.hpp>
static const int tag = dluau_newlightuserdatatag();
constexpr const char* tname = "opaque";

void dluau_pushopaque(lua_State *L, dluau_Opaque pointer) {
    lua_pushlightuserdatatagged(L, pointer, tag);
    lua_setlightuserdataname(L, tag, tname);
}

auto dluau_isopaque(lua_State *L, int idx) -> bool {
    return lua_lightuserdatatag(L, idx) == tag;
}

auto dluau_toopaque(lua_State *L, int idx) -> dluau_Opaque {
    return lua_tolightuserdatatagged(L, idx, tag);
}
auto dluau_checkopaque(lua_State *L, int idx) -> dluau_Opaque {
    if (not dluau_isopaque(L, idx)) dluau::type_error(L, idx, tname);
    return dluau_toopaque(L, idx);
}
