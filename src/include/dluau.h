#pragma once
#include ".dluau_api.h"
#include <luacode.h>
#include <lualib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct lua_State lua_State;
typedef struct luaL_Reg luaL_Reg;
typedef const char* czstring;
typedef char* zstring;
DLUAU_API int dluau_newuserdatatag();
DLUAU_API int dluau_newlightuserdatatag();
DLUAU_API int dluau_gettagfromtname(czstring tname);
DLUAU_API int dluau_registertypetagged(czstring tname);
DLUAU_API bool dluau_istyperegistered(czstring tname);
DLUAU_API void dluau_registertype(czstring tname);
DLUAU_API int dluau_stringatom(lua_State* L, czstring key);
DLUAU_API int dluau_lstringatom(lua_State* L, const char* key, size_t len);
DLUAU_API int dluau_require(lua_State* L, czstring name);
DLUAU_API char* dluau_precompile(const char* src, size_t src_size, size_t* outsize);
DLUAU_API lua_State* dluau_newstate();
struct dluau_runoptions {
    czstring scripts;
    czstring args;
    const luaL_Reg* global_functions;
    int debug_level;
    int optimization_level;
};
DLUAU_API int dluau_run(const dluau_runoptions* opts);
DLUAU_API void dluau_registerglobals(lua_State* L);
DLUAU_API void dluau_openlibs(lua_State* L);
DLUAU_API void dluauopen_dlimport(lua_State* L);
DLUAU_API void dluauopen_print(lua_State* L);
DLUAU_API void dluauopen_scan(lua_State* L);
DLUAU_API void dluauopen_task(lua_State* L);
