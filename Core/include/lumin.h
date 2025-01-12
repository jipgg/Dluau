#pragma once
#include ".lumin_core_api.h"
#include <lua.h>
#include <luacode.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef NDEBUG
#define LUMIN_DEBUG
#define LUMIN_TRACE_CPP_ERROR
#endif

typedef struct lua_State lua_State;
typedef struct luaL_Reg luaL_Reg;
LUMIN_CORE_API int lumin_newtypetag();
LUMIN_CORE_API bool lumin_samemeta(lua_State* L, int idx, const char* tname);
LUMIN_CORE_API int16_t lumin_useratom(const char* key, size_t len);
LUMIN_CORE_API void lumin_loadfuncs(lua_State* L);
LUMIN_CORE_API void luminopen_dll(lua_State* L);
LUMIN_CORE_API lua_CompileOptions* lumin_compileopts;
