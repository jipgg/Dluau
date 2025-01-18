#include "lumin.h"
#include "fs.hpp"
#include <lualib.h>
#include <lib.hpp>
#include <filesystem>
#include <cstdlib>
namespace fs = std::filesystem;

static int current_path(lua_State* L) {
    new_path(L, fs::current_path());
    return 1;
}
static int current_directory(lua_State* L) {
    libfs::new_dir(L, {.path = fs::current_path()});
    return 1;
}
static int temp_directory(lua_State* L) {
    libfs::new_dir(L, {.path = fs::temp_directory_path()});
    return 1;
}
static int find_environment_variable(lua_State* L) {
    if (const char* var = std::getenv(luaL_checkstring(L, 1))) {
        lua_pushstring(L, var);
        return 1;
    }
    return 0;
}
static int file_open(lua_State* L) {
    libfs::new_file(L, {.path = luaL_checkstring(L, 1)});
    return 1;
}
static int directory_open(lua_State* L) {
    libfs::new_dir(L, {.path = luaL_checkstring(L, 1)});
    return 1;
}
static int directory_create(lua_State* L) {
    fs::path p = luaL_checkstring(L, 1);
    try {
        fs::create_directory(p);
    } catch (const fs::filesystem_error& e) {
        luaL_errorL(L, e.what());
    }
    libfs::new_dir(L, {.path = luaL_checkstring(L, 1)});
    return 1;
}
static int path_exists(lua_State* L) {
    lua_pushboolean(L, fs::exists(luaL_checkstring(L, 1)));
    return 1;
}
static int directory_remove(lua_State* L) {
    fs::path p = luaL_checkstring(L, 1);
    bool recursive = luaL_optboolean(L, 2, false);
    if (not fs::exists(p)) luaL_errorL(L, "file doesn't exists.");
    try {
        if (recursive) fs::remove_all(p); 
        else fs::remove(p);
    } catch (const fs::filesystem_error& e) {
        luaL_errorL(L, e.what());
    }
    return 0;
}
static int file_create(lua_State* L) {
    fs::path p = luaL_checkstring(L, 1);
    if (fs::exists(p)) luaL_errorL(L, "file already exists.");
    std::ofstream of{p};
    if (not of.is_open()) luaL_errorL(L, "couldn't open file");
    libfs::new_file(L, {.path = luaL_checkstring(L, 1)});
    return 1;
}
static int file_remove(lua_State* L) {
    fs::path p = luaL_checkstring(L, 1);
    if (not fs::exists(p)) luaL_errorL(L, "file doesn't exists.");
    try {
        fs::remove(p);
    } catch (const fs::filesystem_error& e) {
        luaL_errorL(L, e.what());
    }
    return 0;
}

void luminopen_fs(lua_State* L) {
    const luaL_Reg lib[] = {
        {"current_directory", current_directory},
        {"temp_directory", temp_directory},
        {"find_environment_variable", find_environment_variable},
        {"path_exists", path_exists},
        {nullptr, nullptr}
    };
    const luaL_Reg dirlib[] = {
        {"create", directory_create},
        {"open", directory_open},
        {"remove", directory_remove},
        {nullptr, nullptr}
    };
    const luaL_Reg filelib[] = {
        {"create", file_create},
        {"open", file_open},
        {"remove", file_remove},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    lua_newtable(L);
    luaL_register(L, nullptr, dirlib);
    lua_setfield(L, -2, "Dir");
    lua_newtable(L);
    luaL_register(L, nullptr, filelib);
    lua_setfield(L, -2, "File");
    lua_setglobal(L, "fs");
}
