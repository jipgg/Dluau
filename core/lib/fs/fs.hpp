#include <filesystem>
#include <fstream>
#include <memory>
#include <lualib.h>
#include <iostream>
namespace fs = std::filesystem;
using Path = std::filesystem::path;
struct Directory {
    std::filesystem::path path;
};
struct File {
    std::filesystem::path path;
    std::unique_ptr<std::ifstream> reader;
};
using Reader = std::unique_ptr<std::istream>;
using Writer = std::unique_ptr<std::ostream>;

int reader_line_iterator(lua_State* L);
Reader& new_reader(lua_State* L, Reader&& v);
Reader& check_reader(lua_State* L, int idx);
Directory& new_directory(lua_State* L, const Directory& dir);
Directory& check_directory(lua_State* L, int idx);
File& new_file(lua_State* L, const File& file);
File& check_file(lua_State* L, int idx);
Path& new_path(lua_State* L, const Path& v);
Path& check_path(lua_State* L, int idx);
void newindex_parent(lua_State* L, fs::path& p);
void validate_path(lua_State* L, const fs::path& p);
void standardize(fs::path& p);
