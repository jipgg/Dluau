#include "fs.hpp"
#include <fstream>
#include <algorithm>
namespace fs = std::filesystem;

void newindex_parent(lua_State* L, fs::path &p) {
    fs::path parent;
    if (T_directory::is(L, 3)) {
        parent = T_directory::check(L, 3);
    } else {
        parent = luaL_checkstring(L, 3);
        validate_path(L, parent);
        if (not fs::is_directory(parent)) luaL_errorL(L, "path is not a directory");
    }
    auto newpath = parent / p.filename();
    try {
        fs::rename(p, newpath);
    } catch (const fs::filesystem_error& e) {
        luaL_errorL(L, e.what());
    }
    p = std::move(newpath);
}

void validate_path(lua_State* L, const fs::path &p) {
    if (not fs::exists(p)) luaL_errorL(L, "not a real path");
    if (fs::is_fifo(p)
        or fs::is_other(p)
        or fs::is_socket(p)
        or fs::is_block_file(p)
        or fs::is_character_file(p)
    ) luaL_errorL(L, "file type is unsupported.");
}
void create_new_file(lua_State* L, const fs::path& p) {
    if (fs::exists(p)) dluau::error(L, "file already exists.");
    std::ofstream of{p};
    if (not of.is_open()) dluau::error(L, "couldn't open file");
    T_file::create(L, p);
}
auto to_copy_options(std::string_view str) -> fs::copy_options {
    if (str == "recursive") {
        return fs::copy_options::recursive;
    } else if (str == "update existing") {
        return fs::copy_options::update_existing;
    } else if (str == "skip existing") {
        return fs::copy_options::skip_existing;
    } else if (str == "create symlinks") {
        return fs::copy_options::create_symlinks;
    } else if (str == "copy symlinks") {
        return fs::copy_options::copy_symlinks;
    } else if (str == "skip symlinks") {
        return fs::copy_options::skip_symlinks;
    } else if (str == "overwrite existing") {
        return fs::copy_options::overwrite_existing;
    } else if (str == "directories only") {
        return fs::copy_options::directories_only;
    } else if (str == "create hard links") {
        return fs::copy_options::create_hard_links;
    } else {
        return fs::copy_options::none;
    }
}
