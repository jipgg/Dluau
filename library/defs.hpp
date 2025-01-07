#pragma once
#include <filesystem>
#include <luaconf.h>
#include "halua/common.hpp"
struct lua_State;
namespace filesystem {
int path_ctor(lua_State* L);
void init_path(lua_State* L);
int create_path(lua_State* L, const std::filesystem::path& p);
}
constexpr const char* path_tname{"filesystem.Path"};
