#include "goluau.h"
#include "luminfuncs.h"
#include <lualib.h>
#include <string>
#include <luacode.h>
#include <lua.h>
#include <luaconf.h>
#include <luacodegen.h>
#include <iostream>

static int lua_scan(lua_State* L) {
    std::string in;
    std::cin >> in;
    lua_pushstring(L, in.c_str());
    return 1;
}

void goluau_loadfuncs(lua_State *L) {
    const luaL_Reg global_functions[] = {
        {"loadstring", luminF_loadstring},
        {"require", luminF_require},
        {"collectgarbage", luminF_collectgarbage},
        {"scan", lua_scan},
        {nullptr, nullptr}
    };
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, NULL, global_functions);
    lua_pop(L, 1);
}
void goluau_openlibs(lua_State *L) {
    luaL_openlibs(L);
    goluau_loadfuncs(L);
}

int16_t goluau_useratom(const char* key, size_t size) {
    static int16_t current_number{1};
    return current_number++;
}

int goluau_newudtag() {
    static int curr_type_tag = 1;
    return curr_type_tag++; 
}
int goluau_newludtag() {
    static int curr_type_tag = 1;
    return curr_type_tag++; 
}
bool goluau_samemeta(lua_State *L, int idx, const char* tname) {
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
