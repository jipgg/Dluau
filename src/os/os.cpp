#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "os.hpp"

static int os_execute(lua_State* L) {
    lua_pushinteger(L, std::system(luaL_checkstring(L, 1)));
    return 1;
}
[[noreturn]] static int os_exit(lua_State* L) {
    std::exit(luaL_optinteger(L, 1, 0));
}
static int os_getenv(lua_State* L) {
    const char* env = std::getenv(luaL_checkstring(L, 1));
    if (env == nullptr) return 0;
    lua_pushstring(L, env);
    return 1;
}
static int os_remove(lua_State* L) {
    if (remove(luaL_checkstring(L, 1))) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }
    lua_pushboolean(L, true);
    return 1;
}
static int os_rename(lua_State* L) {
    if (rename(luaL_checkstring(L, 1), luaL_checkstring(L, 2))) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }
    lua_pushboolean(L, true);
    return 1;
}

void dluauopen_os(lua_State* L) {
    lua_getglobal(L, "os");
    if (lua_isnil(L, -1)) luaopen_os(L);
    const luaL_Reg extendedlib[] = {
        {"execute", os_execute},
        {"exit", os_exit},
        {"getenv", os_getenv},
        {"remove", os_remove},
        {"rename", os_rename},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, extendedlib);
    register_windows_lib(L);
    lua_pop(L, 1);
}
