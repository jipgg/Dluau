#pragma once
#include "_lumin_api.h"
#include <stdint.h>
#include <stdbool.h>
#include <lua.h>

LUMINFUNCS_API int luminF_dllimport(lua_State* L);
LUMINFUNCS_API int luminF_loadstring(lua_State* L);
LUMINFUNCS_API int luminF_require(lua_State* L);
LUMINFUNCS_API int luminF_collectgarbage(lua_State* L);
