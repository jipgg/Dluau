#pragma once
#include <filesystem>
#include <fstream>
#include <memory>
#include <lualib.h>
#include <iostream>
#include <memory>
#include <fstream>
#include <lualib.h>
#include <filesystem>
namespace stdfs = std::filesystem;
using Filesystem_path = std::filesystem::path;
Filesystem_path& new_path(lua_State* L, const Filesystem_path& v);
Filesystem_path& check_path(lua_State* L, int idx);
void newindex_parent(lua_State* L, stdfs::path& p);
void validate_path(lua_State* L, const stdfs::path& p);
void standardize(stdfs::path& p);

using Reader = std::unique_ptr<std::istream>;
using Writer = std::unique_ptr<std::ostream>;

int reader_line_iterator(lua_State* L);
Writer& new_writer(lua_State* L, Writer&& v);
Writer& check_writer(lua_State* L, int idx);
Reader& new_reader(lua_State* L, Reader&& v);
Reader& check_reader(lua_State* L, int idx);
struct Dir {std::filesystem::path path;};
struct File {std::filesystem::path path;};
Dir& new_dir(lua_State* L, const Dir& dir);
Dir& check_dir(lua_State* L, int idx);
File& new_file(lua_State* L, const File& file);
File& check_file(lua_State* L, int idx);
