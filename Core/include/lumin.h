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
LUMIN_API int lumin_newuserdatatag();
LUMIN_API int lumin_newlightuserdatatag();
LUMIN_API void lumin_adduserdatatype(const char* tname);
LUMIN_API bool lumin_samemeta(lua_State* L, int idx, const char* tname);
LUMIN_API int16_t lumin_useratom(const char* key, size_t len);
LUMIN_API int lumin_stringatom(lua_State* L, const char* key);
LUMIN_API int lumin_lstringatom(lua_State* L, const char* key, size_t len);
LUMIN_API lua_State* lumin_initstate();
LUMIN_API void lumin_loadfuncs(lua_State* L);
LUMIN_API void lumin_openlibs(lua_State* L);
LUMIN_API void luminopen_dll(lua_State* L);
LUMIN_API void luminopen_fs(lua_State* L);
LUMIN_API int luminload_dll(lua_State* L);
LUMIN_API lua_CompileOptions* lumin_globalcompileoptions;
