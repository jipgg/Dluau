#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "minluau_api.h"

typedef void* halua_Opaque;
typedef const char* czstring;
typedef struct lua_State lua_State;
MINLUAU_API int minluau_newtypetag();
MINLUAU_API bool minluau_samemeta(lua_State* L, int idx, czstring tname);
MINLUAU_API uint32_t* minluau_newuint(lua_State* L, uint32_t uint);
MINLUAU_API uint32_t* minluau_touint(lua_State* L, int idx);
MINLUAU_API bool minluau_isuint(lua_State* L, int idx);
MINLUAU_API halua_Opaque minluau_newopaque(lua_State* L, halua_Opaque opaque);
MINLUAU_API halua_Opaque minluau_toopaque(lua_State* L, int idx);
MINLUAU_API bool minluau_isopaque(lua_State* L, int idx);
