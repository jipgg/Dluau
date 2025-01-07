#ifndef HALUA_LIBAPI_H
#define HALUA_LIBAPI_H
#include <stdint.h>
#include <stdbool.h>
#ifdef HALUA_API_EXPORT
#define HALUA_API extern "C" __declspec(dllexport)
#else
#define HALUA_API extern "C" __declspec(dllimport)
#endif
typedef void* halua_Opaque;
struct lua_State;
HALUA_API int halua_newtypetag();
HALUA_API uint32_t* halua_newuint(lua_State* L, uint32_t uint);
HALUA_API uint32_t* halua_touint(lua_State* L, int idx);
HALUA_API bool halua_isuint(lua_State* L, int idx);
HALUA_API halua_Opaque halua_newopaque(lua_State* L, halua_Opaque opaque);
HALUA_API halua_Opaque halua_toopaque(lua_State* L, int idx);
HALUA_API bool halua_isopaque(lua_State* L, int idx);
#endif
