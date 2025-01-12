#include "_luminstd_api.h"
#include "desktop.hpp"
#include "lumin.h"
#include <iostream>
static int init(lua_State* L) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        luaL_errorL(L, SDL_GetError());
    };
    return 1;
}
static int get_error(lua_State* L) {
    lua_pushstring(L, SDL_GetError());
    return 1;
}
static int quit(lua_State* L) {
    SDL_Quit();
    return 0;
}
static int poll_event(lua_State* L) {
    SDL_Event* event = toevent(L, 1);
    lua_pushboolean(L, SDL_PollEvent(event));
    return 1;
}
LUMINSTD_API inline int luminstd_desktop(lua_State* L) {
    const luaL_Reg functions[] = {
        {"get_error", get_error},
        {"poll_event", poll_event},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    lua_pushinteger(L, SDL_WINDOWPOS_CENTERED);
    lua_setfield(L, -2, "WINDOWPOS_CENTERED");
    lua_pushinteger(L, SDL_WINDOWPOS_UNDEFINED);
    lua_setfield(L, -2, "WINDOWPOS_UNDEFINED");
    register_enums(L);
    register_rect(L);
    register_event(L);
    register_window(L);
    luaL_register(L, nullptr, functions);
    return 1;
}
