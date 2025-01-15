#include "lumin.h"
#include "fs.hpp"
#include <lualib.h>
#include <filesystem>
namespace fs = std::filesystem;

static int current_path(lua_State* L) {
    new_path(L, fs::current_path());
    return 1;
}
static int getcwd(lua_State* L) {
    new_directory(L, {.path = fs::current_path()});
    return 1;
}
static int file(lua_State* L) {
    new_file(L, {.path = luaL_checkstring(L, 1)});
    return 1;
}
static int directory(lua_State* L) {
    new_directory(L, {.path = luaL_checkstring(L, 1)});
    return 1;
}
static int create_directory(lua_State* L) {
    fs::path p = luaL_checkstring(L, 1);
    try {
        fs::create_directory(p);
    } catch (const fs::filesystem_error& e) {
        luaL_errorL(L, e.what());
    }
    new_directory(L, {.path = luaL_checkstring(L, 1)});
    return 1;
}

void luminopen_fs(lua_State* L) {
    const luaL_Reg lib[] = {
        {"getcwd", getcwd},
        {"file", file},
        {"makedir", create_directory},
        {"directory", directory},
        {nullptr, nullptr}
    };
    luaL_register(L, "fs", lib);
}
