#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "minlu_api.h"

typedef void* halua_Opaque;
typedef const char* czstring;
typedef struct lua_State lua_State;
MINLU_API int minlu_newtypetag();
MINLU_API bool minlu_samemeta(lua_State* L, int idx, czstring tname);
MINLU_API uint32_t* minlu_newuint(lua_State* L, uint32_t uint);
MINLU_API uint32_t* minlu_touint(lua_State* L, int idx);
MINLU_API bool minlu_isuint(lua_State* L, int idx);
MINLU_API halua_Opaque minlu_newopaque(lua_State* L, halua_Opaque opaque);
MINLU_API halua_Opaque minlu_toopaque(lua_State* L, int idx);
MINLU_API bool minlu_isopaque(lua_State* L, int idx);
MINLU_API int minlufn_dllimport(lua_State* L);
