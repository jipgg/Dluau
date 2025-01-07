#include "common.hpp"
#include <lualib.h>
#include "halua/libapi.h"
static const int tag = halua_newtypetag();
constexpr const char* type = "SDL2.SDL_Event";

int rect_tag() {
    return tag;
}

static int index(lua_State* L) {
    SDL_Rect& self = *static_cast<SDL_Rect*>(lua_touserdatatagged(L, 1, tag));
    std::string_view key = luaL_checkstring(L, 2);
    if (key.size() != 1) luaL_argerrorL(L, 2, "index is nil");
    switch (key[0]) {
        case 'x':
            lua_pushinteger(L, self.x);
            return 1;
        case 'y':
            lua_pushinteger(L, self.y);
            return 1;
        case 'w':
            lua_pushinteger(L, self.w);
            return 1;
        case 'h':
            lua_pushinteger(L, self.h);
            return 1;
    }
    luaL_argerrorL(L, 2, "index is nil");
}
static int newindex(lua_State* L) {
    SDL_Rect& self = *static_cast<SDL_Rect*>(lua_touserdatatagged(L, 1, tag));
    std::string_view key = luaL_checkstring(L, 2);
    if (key.size() != 1) luaL_argerrorL(L, 2, "index is nil");
    int v = luaL_checkinteger(L, 3);
    switch (key[0]) {
        case 'x':
            self.x = v;
            return 0;
        case 'y':
            self.y = v;
            return 0;
        case 'w':
            self.w = v;
            return 0;
        case 'h':
            self.h = v;
            return 0;
    }
    luaL_argerrorL(L, 2, "index is nil");
}

int rect_ctor_new(lua_State* L) {
    if (luaL_newmetatable(L, type)) {
        const luaL_Reg meta[] = {
            {"__index", index},
            {"__newindex", newindex},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, type);
        lua_setfield(L, -2, "__type");
    }
    lua_pop(L, 1);
    SDL_Rect* p = static_cast<SDL_Rect*>(lua_newuserdatatagged(L, sizeof(SDL_Rect), tag));
    luaL_getmetatable(L, type);
    lua_setmetatable(L, -2);
    *p = SDL_Rect{
        .x = luaL_optinteger(L, 1, 0),
        .y = luaL_optinteger(L, 2, 0),
        .w = luaL_optinteger(L, 3, 0),
        .h = luaL_optinteger(L, 4, 0),
    };
    return 1;
}
void register_rect(lua_State* L) {
    lua_newtable(L);
    lua_pushcfunction(L, rect_ctor_new, "SDL_Rect.new");
    lua_setfield(L, -2, "new");
    lua_setfield(L,-2, "Rect");
}
SDL_Rect* to_rect(lua_State* L, int idx) {
    return static_cast<SDL_Rect*>(lua_touserdatatagged(L, idx, tag));
}
