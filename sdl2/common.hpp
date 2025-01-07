#pragma once
#include <string>
#include <SDL.h>
#include <lualib.h>
#ifdef HALUASDL2_API_EXPORT
#define HALUASDL2_API extern "C" __declspec(dllexport)
#else
#define HALUASDL2_API extern "C" __declspec(dllimport)
#endif

constexpr const char* event_type{"SDL2.Event"};
void event_init(lua_State* L);
int event_tag();
int event_ctor(lua_State* L);
void register_enums(lua_State* L);
void register_rect(lua_State* L);
int rect_tag(lua_State* L);
SDL_Rect* to_rect(lua_State* L, int idx);
