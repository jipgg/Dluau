#pragma once
#include <string>
#include <SDL.h>
#include <lualib.h>

void register_event(lua_State* L);
void push_window_flags(lua_State* L);
void register_enums(lua_State* L);
void register_rect(lua_State* L);
void register_window(lua_State* L);
SDL_Rect* torect(lua_State* L, int idx);
SDL_Event* toevent(lua_State* L, int idx);
