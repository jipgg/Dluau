#include "desktop.hpp"
#include <lualib.h>
#include "class_library.hpp"
#include "lumin.h"
using rectlib = class_library<SDL_Rect>;
using libmap = rectlib::meta_map<std::string>;
int rect_ctor_new(lua_State* L) {
    rectlib::create(L, SDL_Rect{
        .x = luaL_optinteger(L, 1, 0),
        .y = luaL_optinteger(L, 2, 0),
        .w = luaL_optinteger(L, 3, 0),
        .h = luaL_optinteger(L, 4, 0),
    });
    return 1;
}
void register_rect(lua_State* L) {
rectlib::tname = "rect" ;
rectlib::index_map = {
    {"x", [](lua_State* L, SDL_Rect& self) -> int {
        lua_pushinteger(L, self.x);
        return 1;
    }}, {"y", [](lua_State* L, SDL_Rect& self) -> int {
        lua_pushinteger(L, self.y);
        return 1;
    }}, {"w", [](lua_State* L, SDL_Rect& self) -> int {
        lua_pushinteger(L, self.w);
        return 1;
    }}, {"h", [](lua_State* L, SDL_Rect& self) -> int {
        lua_pushinteger(L, self.h);
        return 1;
    }},
};
rectlib::newindex_map = libmap{
    {"x", [](lua_State* L, SDL_Rect& self) -> int {
        int v = luaL_checkinteger(L, 3);
        self.x = v;
        return 0;
    }}, {"y", [](lua_State* L, SDL_Rect& self) -> int {
        int v = luaL_checkinteger(L, 3);
        self.y = v;
        return 0;
    }}, {"w", [](lua_State* L, SDL_Rect& self) -> int {
        int v = luaL_checkinteger(L, 3);
        self.w = v;
        return 0;
    }}, {"h", [](lua_State* L, SDL_Rect& self) -> int {
        int v = luaL_checkinteger(L, 3);
        self.h = v;
        return 0;
    }},

};
    lua_newtable(L);
    lua_pushcfunction(L, rect_ctor_new, "rect.create");
    lua_setfield(L, -2, "create");
    lua_setfield(L,-2, "rect");
}
SDL_Rect* torect(lua_State* L, int idx) {
    return &rectlib::check(L, idx);
}
