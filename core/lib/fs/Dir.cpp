#include <userdata_template.hpp>
#include "fs.hpp"
#include <filesystem>
#include <algorithm>
#include <lib.hpp>
#include <ranges>
namespace fs = std::filesystem;
using Dir_ut = Userdata_template<libfs::Dir>;
using File_ut = Userdata_template<libfs::File>;
static const char* tname = "fsDir";
template<> const char* Dir_ut::type_name(){return tname;}

namespace libfs {
template<class Filesystem_iterator>
static int fs_iterator(lua_State* L) {
    Filesystem_iterator& it = *static_cast<Filesystem_iterator*>(lua_touserdata(L, lua_upvalueindex(1)));
    const Filesystem_iterator end;
    while(it != end) {
        bool pushed = false;
        fs::path p = it->path();
        if (fs::is_directory(p)) {
            new_dir(L, {.path = std::move(p)});
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
            libfs::new_dir(L, {.path = std::move(p)});
            pushed = true;
        } else if (fs::is_regular_file(p)) {
            libfs::new_file(L, {.path = std::move(p)});
            pushed = true;
        }
        ++it;
        if (pushed) return 1;
    }
    return 0;
}

static const Dir_ut::Registry namecall = {
    {"iterator", [](lua_State* L, Dir& s) -> int {
        auto* it = static_cast<fs::directory_iterator*>(lua_newuserdatadtor(L, sizeof(fs::directory_iterator), [](void* data) {
            static_cast<fs::directory_iterator*>(data)->~directory_iterator();
        }));
        new (it) fs::directory_iterator(s.path);
        lua_pushcclosure(L, fs_iterator<fs::directory_iterator>, "directory_iterator", 1);
        return 1;
    }},
    {"recursive_iterator", [](lua_State* L, Dir& s) -> int {
        auto* it = static_cast<fs::recursive_directory_iterator*>(lua_newuserdatadtor(L, sizeof(fs::recursive_directory_iterator), [](void* data) {
            static_cast<fs::recursive_directory_iterator*>(data)->~recursive_directory_iterator();
        }));
        new (it) fs::recursive_directory_iterator(s.path);
        lua_pushcclosure(L, fs_iterator<fs::recursive_directory_iterator>, "recursive_directory_iterator", 1);
        return 1;
    }},
    {"is_empty", [](lua_State* L, Dir& s) -> int {
        lua_pushboolean(L, fs::is_empty(s.path));
        return 1;
    }},
    {"contains", [](lua_State* L, Dir& s) -> int {
        lua_pushboolean(L, fs::exists(s.path / luaL_checkstring(L, 2)));
        return 1;
    }},
};
static const Dir_ut::Registry index = {
    {"parent_directory", [](lua_State* L, Dir& s) -> int {
        new_dir(L, {.path = s.path.parent_path()});
        return 1;
    }},
    {"name", [](lua_State* L, Dir& s) -> int {
        lua_pushstring(L, s.path.filename().string().c_str());
        return 1;
    }},
};
static const Dir_ut::Registry newindex = {
    {"parent_directory", [](lua_State* L, Dir& s) -> int {
        newindex_parent(L, s.path);
        return 0;
    }},
    {"name", [](lua_State* L, Dir& s) -> int {
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
    auto& p = check_dir(L, 1);
    lua_pushstring(L, std::format("{}: {{{}}}", tname, p.path.string()).c_str());
    return 1;
}
static int div(lua_State* L) {
    auto& dir = check_dir(L, 1);
    bool is_directory = false;
    fs::path path;
    if (File_ut::is_type(L, 2)) {
        path = check_dir(L, 2).path;
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
    lua_pushstring(L, result.c_str());
    return 1;
}
Dir& new_dir(lua_State* L, const Dir& v) {
    if (not Dir_ut::initialized(L)) {
        const luaL_Reg meta[] = {
            {"__tostring", tostring},
            {nullptr, nullptr}
        };
        lumin_adduserdatatype(tname);
        Dir_ut::init(L, Dir_ut::init_info{
            .index = index,
            .newindex = newindex,
            .namecall = namecall,
            .meta = meta,
        });
    }
    fs::path path = v.path;
    validate_path(L, path);
    standardize(path);
    return Dir_ut::new_udata(L, {.path = std::move(path)});
}
Dir& check_dir(lua_State* L, int idx) {
    return Dir_ut::check_udata(L, idx);
}
}
