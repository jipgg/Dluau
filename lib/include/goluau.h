#pragma once
#include ".goluau_api.h"
#include <luacode.h>
#include <lualib.h>
#include <stdint.h>
#include <stdbool.h>


typedef struct lua_State lua_State;
typedef struct luaL_Reg luaL_Reg;
GOLUAU_API int goluau_newudtag();
GOLUAU_API int goluau_newludtag();
GOLUAU_API int goluau_gettnametag(const char* tname);
GOLUAU_API int goluau_registertaggedtype(const char* tname);
GOLUAU_API bool goluau_istyperegistered(const char* tname);
GOLUAU_API void goluau_registertype(const char* tname);
GOLUAU_API bool goluau_samemeta(lua_State* L, int idx, const char* tname);
GOLUAU_API int16_t goluau_useratom(const char* key, size_t len);
GOLUAU_API int goluau_stringatom(lua_State* L, const char* key);
GOLUAU_API int goluau_lstringatom(lua_State* L, const char* key, size_t len);
GOLUAU_API lua_State* goluau_initstate();
GOLUAU_API void goluau_init(lua_State* L);
struct goluau_GoOptions {
    const char* scripts;
    const char* args;
    const luaL_Reg* global_functions;
    int debug_level;
    int optimization_level;
};
GOLUAU_API int goluau_go(const goluau_GoOptions* opts);
GOLUAU_API void goluau_loadfuncs(lua_State* L);
GOLUAU_API void goluau_openlibs(lua_State* L);
GOLUAU_API void goluauopen_dll(lua_State* L);
GOLUAU_API int goluauload_dll(lua_State* L);
GOLUAU_API void goluauopen_dynlib(lua_State* L);
GOLUAU_API int goluauload_dynlib(lua_State* L);
GOLUAU_API lua_CompileOptions* goluau_compileoptions;
