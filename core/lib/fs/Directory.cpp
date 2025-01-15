#include <userdata_template.hpp>
#include "fs.hpp"
#include <filesystem>
#include <algorithm>
#include <ranges>
namespace fs = std::filesystem;
using Directory_ut = Userdata_template<Directory>;

using File_ut = Userdata_template<File>;
static const char* tname = "directory";
template<> const char* Directory_ut::type_name(){return tname;}

template<class Filesystem_iterator>
static int fs_iterator(lua_State* L) {
    Filesystem_iterator& it = *static_cast<Filesystem_iterator*>(lua_touserdata(L, lua_upvalueindex(1)));
    const Filesystem_iterator end;
    while(it != end) {
        bool pushed = false;
        fs::path p = it->path();
        if (fs::is_directory(p)) {
            new_directory(L, {.path = std::move(p)});
            pushed = true;
        } else if (fs::is_regular_file(p)) {
            new_file(L, {.path = std::move(p)});
            pushed = true;
        }
        ++it;
        if (pushed) return 1;
    }
    return 0;
}
static int iterator(lua_State* L) {
    fs::directory_iterator& it = *static_cast<fs::directory_iterator*>(lua_touserdata(L, lua_upvalueindex(1)));
    const fs::directory_iterator end;
    while(it != end) {
        bool pushed = false;
        fs::path p = it->path();
        if (fs::is_directory(p)) {
            new_directory(L, {.path = std::move(p)});
            pushed = true;
        } else if (fs::is_regular_file(p)) {
            new_file(L, {.path = std::move(p)});
            pushed = true;
        }
        ++it;
        if (pushed) return 1;
    }
    return 0;
}

static const Directory_ut::Registry namecall = {
    {"getchildren", [](lua_State* L, Directory& d) -> int {
        int i{1};
        lua_newtable(L);
        for (const auto& entry : fs::directory_iterator(d.path)) {
            if (entry.is_directory()) {
                new_directory(L, Directory{.path = entry.path()});
                lua_rawseti(L, -2,  i++);
            } else if (entry.is_regular_file()) {
                new_file(L, {.path = entry.path()});
                lua_rawseti(L, -2, i++);
            }
        }
        return 1;
    }},
    {"iterator", [](lua_State* L, Directory& s) -> int {
        auto* it = static_cast<fs::directory_iterator*>(lua_newuserdatadtor(L, sizeof(fs::directory_iterator), [](void* data) {
            static_cast<fs::directory_iterator*>(data)->~directory_iterator();
        }));
        new (it) fs::directory_iterator(s.path);
        lua_pushcclosure(L, fs_iterator<fs::directory_iterator>, "directory_iterator", 1);
        return 1;
    }},
    {"recursive_iterator", [](lua_State* L, Directory& s) -> int {
        auto* it = static_cast<fs::recursive_directory_iterator*>(lua_newuserdatadtor(L, sizeof(fs::recursive_directory_iterator), [](void* data) {
            static_cast<fs::recursive_directory_iterator*>(data)->~recursive_directory_iterator();
        }));
        new (it) fs::recursive_directory_iterator(s.path);
        lua_pushcclosure(L, fs_iterator<fs::recursive_directory_iterator>, "recursive_directory_iterator", 1);
        return 1;
    }},
};
static const Directory_ut::Registry index = {
    {"parent", [](lua_State* L, Directory& s) -> int {
        new_directory(L, {.path = s.path.parent_path()});
        return 1;
    }},
};
static const Directory_ut::Registry newindex = {
    {"parent", [](lua_State* L, Directory& s) -> int {
        newindex_parent(L, s.path);
        return 0;
    }},
};
static int tostring(lua_State* L) {
    auto& p = check_directory(L, 1);
    lua_pushstring(L, std::format("{}: {{{}}}", tname, p.path.string()).c_str());
    return 1;
}
static int div(lua_State* L) {
    auto& dir = check_directory(L, 1);
    bool is_directory = false;
    fs::path path;
    if (File_ut::is_type(L, 2)) {
        path = check_directory(L, 2).path;
    } else if (File_ut::is_type(L, 2)) {
        path = check_file(L, 2).path;
    } else {
        std::string p = luaL_checkstring(L, 2);
        if (p.ends_with('/')) {
            p.pop_back();
            is_directory = true;
        }
        path = p;
    }
    std::string result = (dir.path / path).string();
    std::ranges::replace(result, '\\', '/');
    if (is_directory) new_directory(L, {.path = result});
    else new_file(L, {.path = result});
    return 1;
}
Directory& new_directory(lua_State* L, const Directory& v) {
    if (not Directory_ut::initialized(L)) {
        const luaL_Reg meta[] = {
            {"__tostring", tostring},
            {nullptr, nullptr}
        };
        lumin_adduserdatatype(tname);
        Directory_ut::init(L, Directory_ut::init_info{
            .index = index,
            .newindex = newindex,
            .namecall = namecall,
            .meta = meta,
        });
    }
    fs::path path = v.path;
    validate_path(L, path);
    standardize(path);
    return Directory_ut::new_udata(L, {.path = std::move(path)});
}
Directory& check_directory(lua_State* L, int idx) {
    return Directory_ut::check_udata(L, idx);
}
