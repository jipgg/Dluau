#include ".lumin_core_api.h"
#include <lua.h>
#include <stdbool.h>

LUMIN_UTILS_API lua_State* luminU_loadscript(lua_State* L, const char* script_path);
LUMIN_UTILS_API void luminU_spawnscript(lua_State* L, const char* script_path);
