#include <lumin.h>
#include <lualib.h>

int lumin_stringatom(lua_State* L, const char *key) {
    lua_pushstring(L, key);
    int atom;
    lua_tostringatom(L, -1, &atom);
    lua_pop(L, 1);
    return atom;
}
int lumin_lstringatom(lua_State* L, const char *key, size_t len) {
    lua_pushlstring(L, key, len);
    int atom;
    lua_tostringatom(L, -1, &atom);
    lua_pop(L, 1);
    return atom;
}
lua_State* lumin_initstate() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    lumin_loadfuncs(L);
    luminopen_dll(L);
    lua_callbacks(L)->useratom = lumin_useratom;
    return L;
}
