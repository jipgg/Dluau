#pragma once
#include ".luauxt_api.h"
#include <luacode.h>
#include <lualib.h>
#include <stdint.h>
#include <stdbool.h>


typedef struct lua_State lua_State;
typedef struct luaL_Reg luaL_Reg;
LUAUXT_API int luauxt_newuserdatatag();
LUAUXT_API int luauxt_newlightuserdatatag();
LUAUXT_API int luauxt_gettagfromtname(const char* tname);
LUAUXT_API int luauxt_registertypetagged(const char* tname);
LUAUXT_API bool luauxt_istyperegistered(const char* tname);
LUAUXT_API void luauxt_registertype(const char* tname);
LUAUXT_API bool luauxt_samemeta(lua_State* L, int idx, const char* tname);
LUAUXT_API int16_t luauxt_defaultuseratom(const char* key, size_t len);
LUAUXT_API int luauxt_stringatom(lua_State* L, const char* key);
LUAUXT_API int luauxt_lstringatom(lua_State* L, const char* key, size_t len);
LUAUXT_API lua_State* luauxt_newstate();
LUAUXT_API void luauxt_init(lua_State* L);
struct goluau_GoOptions {
    const char* scripts;
    const char* args;
    const luaL_Reg* global_functions;
    int debug_level;
    int optimization_level;
};
LUAUXT_API int luauxt_run(const goluau_GoOptions* opts);
LUAUXT_API void luauxt_loadfuncs(lua_State* L);
LUAUXT_API void luauxt_openlibs(lua_State* L);
LUAUXT_API void luauxtopen_dlimport(lua_State* L);
LUAUXT_API int luauxtload_dlimport(lua_State* L);
LUAUXT_API lua_CompileOptions* luauxt_compileoptions;
