#include "fs.hpp"
#include <filesystem>
#include <lua_utility.hpp>
#include <algorithm>
#include <print>
namespace fs = std::filesystem;
using Ty = DirType;

static const Ty::Registry namecall = {
    {"each_child", [](lua_State* L, Path& s) -> int {
        using Iter = fs::directory_iterator;
        new (&lu::new_userdata<Iter>(L)) Iter{s}; 
        lua_pushcclosure(L, fs_iterator<Iter>, "directory_iterator", 1);
        return 1;
    }},
    {"each_descendant", [](lua_State* L, Path& s) -> int {
        using Iter = fs::recursive_directory_iterator;
        new (&lu::new_userdata<Iter>(L)) Iter{s}; 
        lua_pushcclosure(L, fs_iterator<Iter>, "recursive_directory_iterator", 1);
        return 1;
    }},
    {"is_empty", [](lua_State* L, Path& s) -> int {
        lua_pushboolean(L, fs::is_empty(s));
        return 1;
    }},
    {"contains", [](lua_State* L, Path& s) -> int {
        lua_pushboolean(L, fs::exists(s / luaL_checkstring(L, 2)));
        return 1;
    }},
    {"make_file", [](lua_State* L, Path& s) -> int {
        auto p = fs::weakly_canonical(s / luaL_checkstring(L, 2));
        create_new_file(L, p);
        return 1;
    }},
    {"open_file", [](lua_State* L, Path& s) -> int {
        auto p = fs::weakly_canonical(s / luaL_checkstring(L, 2));
        if (not fs::exists(p)) lu::error(L, "path {} does not exist", lua_tostring(L, 2));
        if (not fs::is_regular_file(p)) lu::error(L, "path is not a file");
        FileType::make(L, std::move(p));
        return 1;
    }},
    {"make_directory", [](lua_State* L, Path& s) -> int {
        auto p = fs::weakly_canonical(s / luaL_checkstring(L, 2));
        std::error_code err{};
        if (!fs::create_directory(p, err)) {
            if (!err) lu::error(L, "directory already exists");
            else lu::error(L, err.message());
        }
        DirType::make(L, p);
        return 1;
    }},
    {"open_directory", [](lua_State* L, Path& s) -> int {
        fs::path p = fs::weakly_canonical(s / luaL_checkstring(L, 2));
        if (not fs::exists(p)) lu::error(L, "path {} does not exist", lua_tostring(L, 2));
        if (not fs::is_directory(p)) lu::error(L, "path is not a directory");
        Ty::make(L, std::move(p));
        return 1;
    }}
};
static const Ty::Registry index = {
    {"path", [](lua_State* L, Path& f) -> int {
        lu::push(L, f);
        return 1;
    }},
    {"parent", [](lua_State* L, Path& s) -> int {
        Ty::make(L, s.parent_path());
        return 1;
    }},
    {"name", [](lua_State* L, Path& s) -> int {
        lu::push(L, s.filename());
        return 1;
    }},
};
static const DirType::Registry newindex = {
    {"parent", [](lua_State* L, Path& s) -> int {
        newindex_parent(L, s);
        return 0;
    }},
    {"name", [](lua_State* L, Path& s) -> int {
        auto parent_dir = s.parent_path();
        try {
            fs::rename(s, parent_dir / luaL_checkstring(L, 3));
        } catch (const fs::filesystem_error& e) {
            lu::error(L, e.what());
        }
        return 0;
    }},
};
static auto tostring(lua_State* L) -> int {
    auto& p = Ty::check(L, 1);
    lu::push(L, p.string());
    return 1;
}
static auto div(lua_State* L) -> int {
    auto& dir = Ty::check(L, 1);
    bool is_directory = false;
    fs::path path;
    if (FileType::is(L, 2)) {
        path = Ty::check(L, 2);
    } else if (FileType::is(L, 2)) {
        path = FileType::check(L, 2);
    } else {
        std::string p = luaL_checkstring(L, 2);
        if (p.ends_with('/') or p.ends_with('\\')) {
            p.pop_back();
            is_directory = true;
        }
        path = p;
    }
    std::string result = (dir / path).string();
    lu::push(L, result);
    return 1;
}
constexpr luaL_Reg meta[] = {
    {"__tostring", tostring},
    {"__div", div},
    {nullptr, nullptr}
};
template<> const Ty::InitInfo Ty::init_info {
    .index = index,
    .newindex = newindex,
    .namecall = namecall,
    .meta = meta,
};
