#include "luauxt.h"
#include <lualib.h>
#include <string>
#include <luacode.h>
#include <lua.h>
#include <luaconf.h>
#include <luacodegen.h>
#include <iostream>

void luauxt_openlibs(lua_State *L) {
    luaL_openlibs(L);
    luauxt_loadfuncs(L);
    luauxtopen_dlimport(L);
}

int16_t luauxt_defaultuseratom(const char* key, size_t size) {
    static int16_t current_number{1};
    return current_number++;
}

int luauxt_newuserdatatag() {
    static int curr_type_tag = 1;
    return curr_type_tag++; 
}
int luauxt_newlightuserdatatag() {
    static int curr_type_tag = 1;
    return curr_type_tag++; 
}
bool luauxt_samemeta(lua_State *L, int idx, const char* tname) {
    luaL_getmetatable(L, tname);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return false;
    }
    lua_getmetatable(L, idx);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 2);
        return false;
    }
    const bool eq = lua_rawequal(L, -1, -2);
    lua_pop(L, 2);
    return eq;
}
