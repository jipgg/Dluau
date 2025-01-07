#include "common.hpp"
#include "halua/libapi.h"
#include <iostream>
static int Init(lua_State* L) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        luaL_errorL(L, SDL_GetError());
    };
    return 1;
}
static int GetError(lua_State* L) {
    lua_pushstring(L, SDL_GetError());
    return 1;
}
static int Quit(lua_State* L) {
    SDL_Quit();
    return 0;
}
static int CreateWindow(lua_State* L) {
    std::cout << "creating window\n";
    const char* title = luaL_checkstring(L, 1);
    const int x = luaL_checkinteger(L, 2);
    const int y = luaL_checkinteger(L, 3);
    const int w = luaL_checkinteger(L, 4);
    const int h = luaL_checkinteger(L, 5);
    const int flags = luaL_checkinteger(L, 6);
    SDL_Window* window = SDL_CreateWindow(title, x, y, w, h, flags);
    halua_newopaque(L, window);
    return 1;
}
static int DestroyWindow(lua_State* L) {
    SDL_Window* window = static_cast<SDL_Window*>(halua_toopaque(L, 1));
    SDL_DestroyWindow(window);
    return 0;
}
static int CreateRenderer(lua_State* L) {
    SDL_Window* window = static_cast<SDL_Window*>(halua_toopaque(L, 1));
    const int index = luaL_checkinteger(L, 2);
    const int flags = luaL_checkinteger(L, 3);
    halua_newopaque(L, SDL_CreateRenderer(window, index, flags));
    return 1;
}
static int DestroyRenderer(lua_State* L) {
    SDL_Renderer* renderer = static_cast<SDL_Renderer*>(halua_toopaque(L, 1));
    SDL_DestroyRenderer(renderer);
    return 0;
}
static int RenderClear(lua_State* L) {
    SDL_RenderClear(static_cast<SDL_Renderer*>(halua_toopaque(L, 1)));
    return 0;
}
static int RenderPresent(lua_State* L) {
    SDL_RenderPresent(static_cast<SDL_Renderer*>(halua_toopaque(L, 1)));
    return 0;
}
static int SetRenderDrawColor(lua_State* L) {
    SDL_Renderer* renderer = static_cast<SDL_Renderer*>(halua_toopaque(L, 1));
    SDL_SetRenderDrawColor(renderer, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3), luaL_checkinteger(L, 4), luaL_checkinteger(L, 5));
    return 0;
}
static int PollEvent(lua_State* L) {
    SDL_Event& event = *static_cast<SDL_Event*>(lua_touserdatatagged(L, 1, event_tag()));
    lua_pushboolean(L, SDL_PollEvent(&event));
    return 1;
}
HALUASDL2_API int sdl_library(lua_State* L) {
    const luaL_Reg functions[] = {
        {"Init", Init},
        {"GetError", GetError},
        {"Quit", Quit},
        {"CreateWindow", CreateWindow},
        {"DestroyWindow", DestroyWindow},
        {"CreateRenderer", CreateRenderer},
        {"DestroyRenderer", DestroyRenderer},
        {"RenderClear", RenderClear},
        {"RenderPresent", RenderPresent},
        {"SetRenderDrawColor", SetRenderDrawColor},
        {"PollEvent", PollEvent},
        {"RenderDrawRect", [](lua_State* L) -> int {
            SDL_Renderer* renderer = static_cast<SDL_Renderer*>(halua_toopaque(L, 1));
            SDL_Rect* rect = to_rect(L, 2);
            if (SDL_RenderDrawRect(renderer, rect)) luaL_errorL(L, SDL_GetError());
            return 0;
        }},
        {"RenderFillRect", [](lua_State* L) -> int {
            SDL_Renderer* renderer = static_cast<SDL_Renderer*>(halua_toopaque(L, 1));
            SDL_Rect* rect = to_rect(L, 2);
            if (SDL_RenderFillRect(renderer, rect)) luaL_errorL(L, SDL_GetError());
            return 0;
        }},
        {nullptr, nullptr}
    };
    event_init(L);
    lua_newtable(L);
    lua_pushinteger(L, SDL_WINDOWPOS_CENTERED);
    lua_setfield(L, -2, "WINDOWPOS_CENTERED");
    lua_pushinteger(L, SDL_WINDOWPOS_UNDEFINED);
    lua_setfield(L, -2, "WINDOWPOS_UNDEFINED");
    event_ctor(L);
    lua_setfield(L, -2, "Event");
    register_enums(L);
    register_rect(L);
    luaL_register(L, nullptr, functions);
    return 1;
}
