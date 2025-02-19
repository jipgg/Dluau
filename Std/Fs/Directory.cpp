#include "fs.hpp"
#include <filesystem>
#include <lua_utility.hpp>
#include <algorithm>
#include <print>
namespace fs = std::filesystem;
using Registry = T_directory::Registry;
using Init_info = T_directory::Init_info;

static const Registry namecall = {
    {"each_child", [](lua_State* L, auto& s) -> int {
        using Iter = fs::directory_iterator;
        new (&lu::new_userdata<Iter>(L)) Iter{s}; 
        lua_pushcclosure(L, fs_iterator<Iter>, "directory_iterator", 1);
        return 1;
    }},
    {"each_descendant", [](lua_State* L, auto& s) -> int {
        using Iter = fs::recursive_directory_iterator;
        new (&lu::new_userdata<Iter>(L)) Iter{s}; 
        lua_pushcclosure(L, fs_iterator<Iter>, "recursive_directory_iterator", 1);
        return 1;
    }},
    {"is_empty", [](lua_State* L, auto& s) -> int {
        lua_pushboolean(L, fs::is_empty(s));
        return 1;
    }},
    {"contains", [](lua_State* L, auto& s) -> int {
        lua_pushboolean(L, fs::exists(s / luaL_checkstring(L, 2)));
        return 1;
    }},
    {"make_file", [](lua_State* L, auto& s) -> int {
        auto p = fs::weakly_canonical(s / luaL_checkstring(L, 2));
        create_new_file(L, p);
        return 1;
    }},
    {"open_file", [](lua_State* L, auto& s) -> int {
        auto p = fs::weakly_canonical(s / luaL_checkstring(L, 2));
        if (not fs::exists(p)) lu::error(L, "path {} does not exist", lua_tostring(L, 2));
        if (not fs::is_regular_file(p)) lu::error(L, "path is not a file");
        T_file::make(L, std::move(p));
        return 1;
    }},
    {"make_directory", [](lua_State* L, auto& s) -> int {
        auto p = fs::weakly_canonical(s / luaL_checkstring(L, 2));
        std::error_code err{};
        if (!fs::create_directory(p, err)) {
            if (!err) lu::error(L, "directory already exists");
            else lu::error(L, err.message());
        }
        T_directory::make(L, p);
        return 1;
    }},
    {"open_directory", [](lua_State* L, auto& s) -> int {
        fs::path p = fs::weakly_canonical(s / luaL_checkstring(L, 2));
        if (not fs::exists(p)) lu::error(L, "path {} does not exist", lua_tostring(L, 2));
        if (not fs::is_directory(p)) lu::error(L, "path is not a directory");
        T_directory::make(L, std::move(p));
        return 1;
    }}
};
static const Registry index = {
    {"path", [](lua_State* L, auto& f) -> int {
        lu::push(L, f);
        return 1;
    }},
    {"parent", [](lua_State* L, auto& s) -> int {
        T_directory::make(L, s.parent_path());
        return 1;
    }},
    {"name", [](lua_State* L, auto& s) -> int {
        lu::push(L, s.filename());
        return 1;
    }},
};
static const Registry newindex = {
    {"parent", [](lua_State* L, auto& s) -> int {
        newindex_parent(L, s);
        return 0;
    }},
    {"name", [](lua_State* L, auto& s) -> int {
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
    auto& p = T_directory::check(L, 1);
    lu::push(L, p.string());
    return 1;
}
static auto div(lua_State* L) -> int {
    auto& dir = T_directory::check(L, 1);
    bool is_directory = false;
    fs::path path;
    if (T_file::is(L, 2)) {
        path = T_directory::check(L, 2);
    } else if (T_file::is(L, 2)) {
        path = T_file::check(L, 2);
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
template<> const Init_info T_directory::init_info {
    .index = index,
    .newindex = newindex,
    .namecall = namecall,
    .meta = meta,
};
