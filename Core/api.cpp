#include "lumin.h"
#include "luminfuncs.h"
#include <lualib.h>
#include <string>
#include <luacode.h>
#include <lua.h>
#include <luaconf.h>
#include <luacodegen.h>
[[maybe_unused]] void  export_() {
    volatile auto e= &lua_pushstring;
    volatile auto ee= &luau_load;
    volatile auto eee= &lua_setuserdatadtor;
    volatile auto eeee= &lua_getfield;
}
void lumin_loadfuncs(lua_State *L) {
    static const luaL_Reg global_functions[] = {
        {"loadstring", luminF_loadstring},
        {"require", luminF_require},
        {"collectgarbage", luminF_collectgarbage},
        //{"dllimport", luminF_dllimport},
        {nullptr, nullptr}
    };
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, NULL, global_functions);
    lua_pop(L, 1);
}

int16_t lumin_useratom(const char* key, size_t size) {
    static int16_t current_number{1};
    return current_number++;
}

int lumin_newtypetag() {
    static int curr_type_tag = 1;
    return curr_type_tag++; 
}
bool lumin_samemeta(lua_State *L, int idx, const char* tname) {
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
