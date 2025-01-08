#include "common.hpp"
#include <lualib.h>
#include "minluau.h"
#include <iostream>
#include <unordered_map>
using Namecall = int(*)(lua_State*, SDL_Window*);
static std::unordered_map<int, Namecall> namecalls{};
static const int tag = halua_newtypetag();
constexpr const char* tname = "SDL_Window";
using Window_ptr = SDL_Window*;

static int namecall(lua_State* L) {
    Window_ptr window = *static_cast<Window_ptr*>(lua_touserdatatagged(L, 1, tag));
    int atom;
    lua_namecallatom(L, &atom);
    auto found_it = namecalls.find(atom);
    if (found_it == namecalls.end()) luaL_errorL(L, "invalid namecall");
    return found_it->second(L, window);
}
static int add_namecall(lua_State* L, std::string_view key, Namecall call) {
    lua_pushlstring(L, key.data(), key.size());
    int atom;
    lua_tostringatom(L, -1, &atom);
    lua_pop(L, 1);
    namecalls.emplace(atom, call);
    return atom;
}
static int getPosition(lua_State* L, SDL_Window* window) {
    int x, y;
    SDL_GetWindowPosition(window, &x, &y);
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    return 2;
}
static int setPosition(lua_State* L, SDL_Window* window) {
    SDL_SetWindowPosition(window, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3));
    return 0;
}
static int getSize(lua_State* L, SDL_Window* window) {
    int w, h;
    SDL_GetWindowPosition(window, &w, &h);
    lua_pushinteger(L, w);
    lua_pushinteger(L, h);
    return 2;
}
static int setSize(lua_State* L, SDL_Window* window) {
    SDL_SetWindowSize(window, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3));
    return 0;
}
static int setOpacity(lua_State* L, SDL_Window* window) {
    SDL_SetWindowOpacity(window, static_cast<float>(luaL_checknumber(L, 2)));
    return 0;
}
static int getOpacity(lua_State* L, SDL_Window* window) {
    float opacity;
    SDL_GetWindowOpacity(window, &opacity);
    lua_pushnumber(L, opacity);
    return 1;
}
int window_new(lua_State* L) {
    Window_ptr& window = *static_cast<Window_ptr*>(lua_newuserdatatagged(L, sizeof(Window_ptr), tag));
    const char* title = luaL_checkstring(L, 1);
    const int x = luaL_checkinteger(L, 2);
    const int y = luaL_checkinteger(L, 3);
    const int w = luaL_checkinteger(L, 4);
    const int h = luaL_checkinteger(L, 5);
    const int flags = luaL_checkinteger(L, 6);
    window = SDL_CreateWindow(title, x, y, w, h, flags);
    if (window == nullptr) luaL_errorL(L, SDL_GetError());
    luaL_getmetatable(L, tname);
    lua_setmetatable(L, -2);
    return 1;
}

void register_window(lua_State* L) {
    const luaL_Reg meta[] = {
        {"__namecall", namecall},
        {nullptr, nullptr}
    };
    luaL_newmetatable(L, tname);
    luaL_register(L, nullptr, meta);
    add_namecall(L, "getPosition", getPosition);
    add_namecall(L, "setPosition", setPosition);
    add_namecall(L, "getSize", getSize);
    add_namecall(L, "setSize", setSize);
    add_namecall(L, "getOpacity", getOpacity);
    add_namecall(L, "setOpacity", setOpacity);
    lua_pop(L, 1);
    lua_setuserdatadtor(L, tag, [](lua_State* L, void* ud){
        SDL_DestroyWindow(*static_cast<Window_ptr*>(ud));
        std::cout << "Destroying window\n";
    });
    lua_newtable(L);
    lua_pushcfunction(L, window_new, "window_ctor");
    lua_setfield(L, -2, "create");
    push_window_flags(L);
    lua_setfield(L, -2, "flags");
    lua_setfield(L, -2, "window");
}
