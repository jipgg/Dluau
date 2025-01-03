#pragma once
#include <filesystem>
struct lua_State;
namespace filesystem {
int path_ctor(lua_State* L);
void init_path(lua_State* L);
int create_path(lua_State* L, const std::filesystem::path& p);
constexpr int path_tag{2};
}
constexpr const char* types_key = "_TYPES";
