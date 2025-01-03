#include "api.hpp"
#include <lualib.h>
#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;

HALUA_FILESYSTEM_API int absolute(lua_State* L) {
    auto path = fs::absolute(luaL_checkstring(L, 1));
    lua_pushstring(L, path.string().c_str());
    return 1;
}
HALUA_FILESYSTEM_API int current_path(lua_State* L) {
    lua_pushstring(L, fs::current_path().string().c_str());
    return 1;
}
HALUA_FILESYSTEM_API int temp_directory_path(lua_State* L) {
    lua_pushstring(L, fs::temp_directory_path().string().c_str());
    return 1;
}
HALUA_FILESYSTEM_API int relative(lua_State* L) {
    lua_pushstring(L, fs::relative(luaL_checkstring(L, 1), luaL_optstring(L, 2, fs::current_path().string().c_str())).string().c_str());
    return 1;
}
HALUA_FILESYSTEM_API int children(lua_State* L) {
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
HALUA_FILESYSTEM_API int descendants(lua_State* L) {
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
HALUA_FILESYSTEM_API int is_directory(lua_State* L) {
    lua_pushboolean(L, fs::is_directory(luaL_checkstring(L, 1)));
    return 1;
}
HALUA_FILESYSTEM_API int is_fifo(lua_State* L) {
    lua_pushboolean(L, fs::is_fifo(luaL_checkstring(L, 1)));
    return 1;
}
HALUA_FILESYSTEM_API int is_regular_file(lua_State* L) {
    lua_pushboolean(L, fs::is_regular_file(luaL_checkstring(L, 1)));
    return 1;
}
HALUA_FILESYSTEM_API int is_character_file(lua_State* L) {
    lua_pushboolean(L, fs::is_character_file(luaL_checkstring(L, 1)));
    return 1;
}
HALUA_FILESYSTEM_API int is_block_file(lua_State* L) {
    lua_pushboolean(L, fs::is_block_file(luaL_checkstring(L, 1)));
    return 1;
}
HALUA_FILESYSTEM_API int is_symlink(lua_State* L) {
    lua_pushboolean(L, fs::is_symlink(luaL_checkstring(L, 1)));
    return 1;
}
HALUA_FILESYSTEM_API int exists(lua_State* L) {
    lua_pushboolean(L, fs::exists(luaL_checkstring(L, 1)));
    return 1;
}
