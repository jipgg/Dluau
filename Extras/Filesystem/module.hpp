#pragma once
#include <filesystem>
#include <luaconf.h>
struct lua_State;
using Filesystem_path = std::filesystem::path;
int path_ctor(lua_State* L);
void init_path(lua_State* L);
int create_path(lua_State* L, const std::filesystem::path& p);
constexpr const char* path_tname{"filesystem.Path"};
