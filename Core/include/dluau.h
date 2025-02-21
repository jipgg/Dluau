#pragma once
#ifdef DLUAUCORE_EXPORT
#define DLUAU_API extern "C" __declspec(dllexport)
#else
#define DLUAU_API extern "C" __declspec(dllimport)
#endif
#include <luacode.h>
#include <lualib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct lua_State lua_State;
typedef struct luaL_Reg luaL_Reg;
DLUAU_API int dluau_newuserdatatag();
DLUAU_API int dluau_newlightuserdatatag();
DLUAU_API int dluau_gettagfromtype(const char* tname);
DLUAU_API int dluau_registertypetagged(const char* tname);
DLUAU_API bool dluau_istyperegistered(const char* tname);
DLUAU_API void dluau_registertype(const char* tname);
DLUAU_API const char* dluau_findstringatom(int atom);
DLUAU_API int dluau_stringatom(lua_State* L, const char* key);
DLUAU_API int dluau_lstringatom(lua_State* L, const char* key, size_t len);
struct dluau_RunOptions {
    const char* scripts;
    const char* args;
    const luaL_Reg* global_functions;
    int debug_level;
    int optimization_level;
};
DLUAU_API int dluau_run(const dluau_RunOptions* opts);
enum dluau_CTaskStatus {
    DLUAU_CTASK_DONE,
    DLUAU_CTASK_CONTINUE,
    DLUAU_CTASK_ERROR,
};
typedef dluau_CTaskStatus(*dluau_CTask)(const char** errmsg);
DLUAU_API void dluau_addctask(dluau_CTask step_callback);
DLUAU_API bool dluau_tasksinprogress();
DLUAU_API bool dluau_taskstep(lua_State* L);

typedef void* dluau_Opaque;
DLUAU_API void dluau_pushopaque(lua_State* L, dluau_Opaque pointer);
DLUAU_API dluau_Opaque dluau_checkopaque(lua_State* L, int idx);
DLUAU_API dluau_Opaque dluau_toopaque(lua_State* L, int idx);
DLUAU_API bool dluau_isopaque(lua_State* L, int idx);
struct dluau_Dlmodule;

DLUAU_API dluau_Dlmodule* dluau_todlmodule(lua_State* L, int idx);
DLUAU_API void dluau_pushdlmodule(lua_State* L, dluau_Dlmodule* dlm);
DLUAU_API uintptr_t dluau_dlfindprocaddress(dluau_Dlmodule* dlm, const char* symbol);
DLUAU_API dluau_Dlmodule* dluau_getdlmodule(lua_State* L, const char* require_path);
