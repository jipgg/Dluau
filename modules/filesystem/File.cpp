#include <TaggedType.hpp>
#include "fs.hpp"
#include <filesystem>
#include <algorithm>
#include <ranges>
namespace fs = std::filesystem;
using FileType = TaggedType<File>;
using DirType = TaggedType<Dir>;
static const char* tname = "fs_File";
template<> const char* FileType::type_name(){return tname;}

static const FileType::Registry namecall = {
    {"reader", [](lua_State* L, File& s) -> int {
        auto f = std::make_unique<std::ifstream>();
        f->open(s.path);
        if (not f->is_open()) luaL_errorL(L, "couldn't open file");
        new_reader(L, std::move(f));
        return 1;
    }},
    {"writer", [](lua_State* L, File& s) -> int {
        auto f = std::make_unique<std::ofstream>();
        bool appendmode = luaL_optboolean(L, 2, false);
        std::cout << std::format("appendmode: {}\n", appendmode);
        if (appendmode) {
            f->open(s.path, std::ios::app);
        } else f->open(s.path);
        if (not f->is_open()) luaL_errorL(L, "couldn't open file");
        new_writer(L, std::move(f));
        return 1;
    }},
};
static const FileType::Registry index = {
    {"parent_directory", [](lua_State* L, File& s) -> int {
        new_dir(L, {.path = s.path.parent_path()});
        return 1;
    }},
    {"stem", [](lua_State* L, File& s) -> int {
        lua_pushstring(L, s.path.stem().string().c_str());
        return 1;
    }},
    {"extension", [](lua_State* L, File& s) -> int {
        lua_pushstring(L, s.path.extension().string().c_str());
        return 1;
    }},
    {"name", [](lua_State* L, File& s) -> int {
        lua_pushstring(L, s.path.filename().string().c_str());
        return 1;
    }},
};
static const FileType::Registry newindex = {
    {"parent_directory", [](lua_State* L, File& s) -> int {
        newindex_parent(L, s.path);
        return 0;
    }},
    {"name", [](lua_State* L, File& s) -> int {
        auto parent_dir = s.path.parent_path();
        try {
            fs::rename(s.path, parent_dir / luaL_checkstring(L, 3));
        } catch (const fs::filesystem_error& e) {
            luaL_errorL(L, e.what());
        }
        return 0;
    }},
};
static int tostring(lua_State* L) {
    auto& p = check_file(L, 1);
    lua_pushstring(L, std::format("{}: {{{}}}", tname, p.path.string()).c_str());
    return 1;
}
File& new_file(lua_State* L, const File& v) {
    if (not FileType::initialized(L)) {
        const luaL_Reg meta[] = {
            {"__tostring", tostring},
            {nullptr, nullptr}
        };
        FileType::init(L, {
            .index = index,
            .newindex = newindex,
            .namecall = namecall,
            .meta = meta,
        });
    }
    fs::path path = v.path;
    validate_path(L, path);
    standardize(path);
    return FileType::new_udata(L, {.path = std::move(path)});
}
File& check_file(lua_State* L, int idx) {
    return FileType::check_udata(L, idx);
}
