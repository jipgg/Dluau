#pragma once
#include <stdint.h>
#include <stdbool.h>
#ifdef MINLUAU_API_EXPORT
#define MINLUAU_API extern "C" __declspec(dllexport)
#else
#define MINLUAU_API extern "C" __declspec(dllimport)
#endif
typedef void* halua_Opaque;
typedef const char* czstring;
typedef struct lua_State lua_State;
MINLUAU_API int halua_newtypetag();
MINLUAU_API bool halua_samemeta(lua_State* L, int idx, czstring tname);
MINLUAU_API uint32_t* halua_newuint(lua_State* L, uint32_t uint);
MINLUAU_API uint32_t* halua_touint(lua_State* L, int idx);
MINLUAU_API bool halua_isuint(lua_State* L, int idx);
MINLUAU_API halua_Opaque halua_newopaque(lua_State* L, halua_Opaque opaque);
MINLUAU_API halua_Opaque halua_toopaque(lua_State* L, int idx);
MINLUAU_API bool halua_isopaque(lua_State* L, int idx);
