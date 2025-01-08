#include "minluau.h"
#include <lualib.h>
#include <luacode.h>
#include <lua.h>
#include <luaconf.h>
#include <luacodegen.h>

[[maybe_unused]] void export_forwarding_ensurance(lua_State* L) {
    volatile auto a = luaL_newstate;
    volatile auto a1 = luaL_sandbox;
    volatile auto a2 = luau_compile;
}
