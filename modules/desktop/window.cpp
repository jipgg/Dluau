#include "desktop.hpp"
#include <lualib.h>
#include <string_view>
#include "minluau.h"
#include <iostream>
#include <unordered_map>
using Namecall = int(*)(lua_State*, SDL_Window*);
static std::unordered_map<int, Namecall> namecalls{};
static const int tag = minluau_newtypetag();
constexpr const char* tname = "desktop.window";

static int index(lua_State* L) {
    window_ptr window = *towindow(L, 1);
    const std::string_view key = luaL_checkstring(L, 2); 
    switch(key[0]) {
        case 'o': {
            float opacity;
            SDL_GetWindowOpacity(window, &opacity);
            lua_pushnumber(L, opacity);
            return 1;
        } case 't': {
            lua_pushstring(L, SDL_GetWindowTitle(window));
            return 1;
        }
    }
    luaL_errorL(L, "invalid index");
}
static int newindex(lua_State* L) {
    window_ptr window = *towindow(L, 1);
    const std::string_view key = luaL_checkstring(L, 2); 
    switch(key[0]) {
        case 'o': {
            const double opacity = luaL_checknumber(L, 3);
            SDL_SetWindowOpacity(window, static_cast<float>(opacity));
            return 0;
        } case 't': {
            const char* title = luaL_checkstring(L, 3);
            SDL_SetWindowTitle(window, title);
            return 0;
        }
    }
    luaL_errorL(L, "invalid index");
}

static int namecall(lua_State* L) {
    window_ptr window = *static_cast<window_ptr*>(lua_touserdatatagged(L, 1, tag));
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
static int position(lua_State* L, SDL_Window* window) {
    if (lua_isnone(L, 2)) {
        int x, y;
        SDL_GetWindowPosition(window, &x, &y);
        lua_pushinteger(L, x);
        lua_pushinteger(L, y);
        return 2;
    }
    SDL_SetWindowPosition(window, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3));
    return 0;
}
static int size(lua_State* L, SDL_Window* window) {
    if (lua_isnone(L, 2)) {
        int w, h;
        SDL_GetWindowPosition(window, &w, &h);
        lua_pushinteger(L, w);
        lua_pushinteger(L, h);
        return 2;
    }
    SDL_SetWindowSize(window, luaL_checkinteger(L, 2), luaL_checkinteger(L, 3));
    return 0;
}
int window_new(lua_State* L) {
    window_ptr& window = *static_cast<window_ptr*>(lua_newuserdatatagged(L, sizeof(window_ptr), tag));
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
window_ptr* towindow(lua_State* L, int idx) {
    return static_cast<window_ptr*>(lua_touserdatatagged(L, idx, tag));
}

void register_window(lua_State* L) {
    const luaL_Reg meta[] = {
        {"__namecall", namecall},
        {"__index", index},
        {"__newindex", newindex},
        {nullptr, nullptr}
    };
    luaL_newmetatable(L, tname);
    luaL_register(L, nullptr, meta);
    add_namecall(L, "position", position);
    add_namecall(L, "size", size);
    lua_pop(L, 1);
    lua_setuserdatadtor(L, tag, [](lua_State* L, void* ud){
        SDL_DestroyWindow(*static_cast<window_ptr*>(ud));
    });
    lua_newtable(L);
    lua_pushcfunction(L, window_new, "window_ctor");
    lua_setfield(L, -2, "create");
    push_window_flags(L);
    lua_setfield(L, -2, "FLAGS");
    lua_setfield(L, -2, "window");
}
