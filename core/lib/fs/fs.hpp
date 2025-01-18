#include <filesystem>
#include <fstream>
#include <memory>
#include <lualib.h>
#include <iostream>
namespace stdfs = std::filesystem;
using Filesystem_path = std::filesystem::path;
Filesystem_path& new_path(lua_State* L, const Filesystem_path& v);
Filesystem_path& check_path(lua_State* L, int idx);
void newindex_parent(lua_State* L, stdfs::path& p);
void validate_path(lua_State* L, const stdfs::path& p);
void standardize(stdfs::path& p);
