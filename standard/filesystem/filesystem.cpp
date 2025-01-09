#include "stdminlu_api.h"
#include <lualib.h>
#include <filesystem>
#include "minlu.h"
#include "filesystem.hpp"
namespace fs = std::filesystem;

static int absolute(lua_State* L) {
    auto path = fs::absolute(luaL_checkstring(L, 1));
    lua_pushstring(L, path.string().c_str());
    return 1;
}
static int current_path(lua_State* L) {
    lua_pushstring(L, fs::current_path().string().c_str());
    return 1;
}
static int temp_directory_path(lua_State* L) {
    lua_pushstring(L, fs::temp_directory_path().string().c_str());
    return 1;
}
static int relative(lua_State* L) {
    lua_pushstring(L, fs::relative(luaL_checkstring(L, 1), luaL_optstring(L, 2, fs::current_path().string().c_str())).string().c_str());
    return 1;
}
static int directory_iterator(lua_State* L) {
    int i{1};
    lua_newtable(L);
    for (const auto& entry : fs::directory_iterator(luaL_checkstring(L, 1))) {
        auto path = entry.path();
        auto str = path.string();
        lua_pushstring(L, str.c_str());
        lua_rawseti(L, -2, i++);
    }
    return 1;
}
static int recursive_directory_iterator(lua_State* L) {
    int i{1};
    lua_newtable(L);
    for (const auto& entry : fs::recursive_directory_iterator(luaL_checkstring(L, 1))) {
        auto path = entry.path();
        auto str = path.string();
        lua_pushstring(L, str.c_str());
        lua_rawseti(L, -2, i++);
    }
    return 1;
}
static int is_directory(lua_State* L) {
    lua_pushboolean(L, fs::is_directory(luaL_checkstring(L, 1)));
    return 1;
}
static int is_fifo(lua_State* L) {
    lua_pushboolean(L, fs::is_fifo(luaL_checkstring(L, 1)));
    return 1;
}
static int is_regular_file(lua_State* L) {
    lua_pushboolean(L, fs::is_regular_file(luaL_checkstring(L, 1)));
    return 1;
}
static int is_character_file(lua_State* L) {
    lua_pushboolean(L, fs::is_character_file(luaL_checkstring(L, 1)));
    return 1;
}
static int is_block_file(lua_State* L) {
    lua_pushboolean(L, fs::is_block_file(luaL_checkstring(L, 1)));
    return 1;
}
static int is_symlink(lua_State* L) {
    lua_pushboolean(L, fs::is_symlink(luaL_checkstring(L, 1)));
    return 1;
}
static int exists(lua_State* L) {
    lua_pushboolean(L, fs::exists(luaL_checkstring(L, 1)));
    return 1;
}
static const luaL_Reg library[] = {
    {"absolute", absolute},
    {"current_path", current_path},
    {"temp_directory_path", temp_directory_path},
    {"relative", relative},
    {"directory_iterator", directory_iterator},
    {"recursive_directory_iterator", recursive_directory_iterator},
    {"is_directory", is_directory},
    {"path", path_ctor},
    {nullptr, nullptr}
};
STDMINLU_API inline int stdminlu_filesystem(lua_State* L) {
    init_path(L);
    lua_newtable(L);
    luaL_register(L, nullptr, library);
    return 1;
}
