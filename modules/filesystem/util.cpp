#include "fs.hpp"
#include "TaggedType.hpp"
#include <algorithm>
namespace fs = std::filesystem;
using FileType = TaggedType<File>;
using DirType = TaggedType<Dir>;

void newindex_parent(lua_State* L, fs::path &p) {
    fs::path parent;
    if (DirType::is_type(L, 3)) {
        parent = check_dir(L, 3).path;
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
void standardize(fs::path &p) {
    std::string str = fs::absolute(p).string();
    std::ranges::replace(str, '\\', '/');
    p = std::move(str);
}
