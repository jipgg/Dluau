#include "minlu.h"
#include <lualib.h>
#include <string>
#include <luacode.h>
#include <lua.h>
#include <luaconf.h>
#include <luacodegen.h>

int minlu_newtypetag() {
    static int curr_type_tag = 1;
    return curr_type_tag++; 
}

static int uint_tag = minlu_newtypetag();

static int uint_tostring(lua_State* L) {
    lua_pushstring(L, std::to_string(*minlu_touint(L, 1)).c_str());
    return 1;
}

bool minlu_isuint(lua_State* L, int idx) {
    return lua_userdatatag(L, idx) == uint_tag;
}

uint32_t* minlu_touint(lua_State* L, int idx) {
    return static_cast<uint32_t*>(lua_touserdatatagged(L, idx, uint_tag));
}

uint32_t* minlu_newuint(lua_State *L, uint32_t uint) {
    if (luaL_newmetatable(L, "uint")) {
        const luaL_Reg meta[] = {
            {"__tostring", uint_tostring},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, "uint");
        lua_setfield(L, -2, "__type");
    }
    lua_pop(L, 1);
    uint32_t* p = static_cast<uint32_t*>(lua_newuserdatatagged(L, sizeof(uint32_t), uint_tag));
    luaL_getmetatable(L, "uint");
    lua_setmetatable(L, -2);
    *p = uint;
    return p;
}
static const int opaque_tag = minlu_newtypetag();
halua_Opaque minlu_newopaque(lua_State* L, halua_Opaque opaque) {
    halua_Opaque* p = static_cast<halua_Opaque*>(lua_newuserdatatagged(L, sizeof(halua_Opaque), opaque_tag));
    *p = opaque;
    return *p;
}
halua_Opaque minlu_toopaque(lua_State *L, int idx) {
    halua_Opaque* p = static_cast<halua_Opaque*>(lua_touserdatatagged(L, idx, opaque_tag));
    return *p;
}
bool minlu_isopaque(lua_State *L, int idx) {
    return lua_userdatatag(L, idx) == opaque_tag;
}
bool minlu_samemeta(lua_State *L, int idx, czstring tname) {
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
