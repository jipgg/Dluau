#include "common.hpp"
#include <lualib.h>
#include "halua/libapi.h"
#include <typeinfo>

namespace halua {

template <class T>
int get_tag() {
    static const int tag = halua_newtypetag();
    return tag;
}
template <class T>
const char* get_name() {
    return typeid(T).raw_name();
}
template <class T>
[[nodiscard]] T& new_userdata(lua_State* L, const T& v = T{}) {
    T* ud = static_cast<T*>(lua_newuserdatatagged(L, sizeof(T), get_tag<T>()));
    new (ud) T{v};
    luaL_getmetatable(L, get_name<T>());
    if (not lua_isnil(L, -1)) {
        lua_setmetatable(L, -2);
    } else {
        lua_pop(L, 1);
    }
    return *ud;
}
template <class T>
T& push_light_userdata(lua_State* L, const T* ptr) {
    T* ud = static_cast<T*>(lua_pushlightuserdatatagged(L, ptr, get_tag<T>()));
    luaL_getmetatable(L, get_name<T>());
    if (not lua_isnil(L, -1)) {
        lua_setmetatable(L, -2);
    } else {
        lua_pop(L, 1);
    }
    return *ud;
}
template <class T>
[[nodiscard]] T& to_userdata(lua_State* L, int idx) {
    if (lua_islightuserdata(L, idx)) {
        void* ud = lua_tolightuserdatatagged(L, idx, get_tag<T>());
        return *static_cast<T*>(ud);
    }
    void* ud = lua_touserdatatagged(L, idx, get_tag<T>());
    return *static_cast<T*>(ud);
}
}
