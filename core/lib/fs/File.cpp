
#include <Generic_userdata_template.hpp>
#include "fs.hpp"
#include <filesystem>
#include <algorithm>
#include <ranges>
namespace fs = std::filesystem;
using File_meta = Generic_userdata_template<File>;
using Directory_meta = Generic_userdata_template<Directory>;
static const char* tname = "file";
template<> const char* File_meta::type_name(){return tname;}

static const File_meta::Registry namecall = {
    {"reader", [](lua_State* L, File& s) -> int {
        auto f = std::make_unique<std::ifstream>();
        f->open(s.path);
        if (not f->is_open()) luaL_errorL(L, "couldn't open file");
        new_reader(L, std::move(f));
        return 1;
    }},
    {"eachline", [](lua_State* L, File& s) -> int {
        auto f = std::make_unique<std::ifstream>();
        f->open(s.path);
        if (not f->is_open()) luaL_errorL(L, "couldn't open file");
        new_reader(L, std::move(f));
        lua_pushcclosure(L, reader_line_iterator, "line_iterator", 1);
        return 1;
    }},
};
static const File_meta::Registry index = {
    {"parent", [](lua_State* L, File& s) -> int {
        new_directory(L, {.path = s.path.parent_path()});
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
    {"filename", [](lua_State* L, File& s) -> int {
        lua_pushstring(L, s.path.filename().string().c_str());
        return 1;
    }},
};
static const File_meta::Registry newindex = {
    {"parent", [](lua_State* L, File& s) -> int {
        newindex_parent(L, s.path);
        return 0;
    }},
};
static int tostring(lua_State* L) {
    auto& p = check_file(L, 1);
    lua_pushstring(L, std::format("{}: {{{}}}", tname, p.path.string()).c_str());
    return 1;
}
File& new_file(lua_State* L, const File& v) {
    if (not File_meta::initialized(L)) {
        const luaL_Reg meta[] = {
            {"__tostring", tostring},
            {nullptr, nullptr}
        };
        lumin_adduserdatatype(tname);
        File_meta::init(L, {
            .index = index,
            .newindex = newindex,
            .namecall = namecall,
            .meta = meta,
        });
    }
    fs::path path = v.path;
    validate_path(L, path);
    standardize(path);
    return File_meta::new_udata(L, {.path = std::move(path)});
}
File& check_file(lua_State* L, int idx) {
    return File_meta::check_udata(L, idx);
}
