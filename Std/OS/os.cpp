#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "os.hpp"
#include <span>
#include <print>
#include <thread>
#include <boost/container/flat_map.hpp>
using std::span;
using boost::container::flat_map;

static auto execute(lua_State* L) -> int {
    lua_pushinteger(L, std::system(luaL_checkstring(L, 1)));
    return 1;
}
[[noreturn]] static auto exit(lua_State* L) -> int {
    std::exit(luaL_optinteger(L, 1, 0));
}
static auto getenv(lua_State* L) -> int {
    const char* env = std::getenv(luaL_checkstring(L, 1));
    if (env == nullptr) return 0;
    lua_pushstring(L, env);
    return 1;
}
static auto remove(lua_State* L) -> int {
    if (remove(luaL_checkstring(L, 1))) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }
    lua_pushboolean(L, true);
    return 1;
}
static auto rename(lua_State* L) -> int {
    if (rename(luaL_checkstring(L, 1), luaL_checkstring(L, 2))) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }
    lua_pushboolean(L, true);
    return 1;
}
consteval auto os_name() -> const char* {
#if defined(_WIN32)
    return "windows";
    #elif defined(__linux__)
        return "linux";
    #elif defined(__APPLE__) && defined(__MACH__)
        return "macOS";
    #elif defined(__FreeBSD__)
    return "freeBSD";
    #else
        return "unknown";
    #endif
}


static auto sleep(lua_State* L) -> int {
    std::this_thread::sleep_for(std::chrono::milliseconds(luaL_checkinteger(L, 1)));
    return 0;
}

DLUAUSTD_API auto dlrequire(lua_State* L) -> int {
    const luaL_Reg extendedlib[] = {
        {"execute", execute},
        {"exit", exit},
        {"getenv", getenv},
        {"remove", remove},
        {"rename", rename},
        {"sleep", sleep},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, extendedlib);
    lua_pushstring(L, os_name());
    lua_setfield(L, -2, "platform");
    register_windows_lib(L);
    lua_setreadonly(L, -1, true);
    return 1;
}
