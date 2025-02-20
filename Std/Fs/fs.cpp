#include "dluau.h"
#include <fstream>
#include "fs.hpp"
#include "fs.hpp"
#include <filesystem>
#include <cstdlib>
namespace fs = std::filesystem;

static auto current_directory(lua_State* L) -> int {
    T_directory::make(L, fs::current_path());
    return 1;
}
static auto canonical(lua_State* L) -> int {
    try {
        fs::path path = fs::canonical(luaL_checkstring(L, 1));
        dluau::push(L, path.string());
        return 1;
    } catch (fs::filesystem_error& err) {
        dluau::error(L, "{}: p1:{}, p2:{}",
            err.what(), err.path1().string(),
            err.path2().string()
        );
    }
}
static auto weakly_canonical(lua_State* L) -> int {
    fs::path path = fs::weakly_canonical(luaL_checkstring(L, 1));
    dluau::push(L, path.string());
    return 1;
}
static auto absolute(lua_State* L) -> int {
    fs::path path = fs::absolute(luaL_checkstring(L, 1));
    dluau::push(L, path.string());
    return 1;
}
static auto each_child(lua_State* L) -> int {
    fs::path path = luaL_checkstring(L, 1);
    if (not fs::is_directory(path)) luaL_errorL(L, "not a directory '%s'", lua_tostring(L, 1));
    using Iter = fs::directory_iterator;
    dluau::make_userdata<Iter>(L, Iter(path));
    //new (&lu::newuserdata<Iter>(L)) Iter(path);
    lua_pushcclosure(L, fs_iterator<Iter>, "directory_iterator", 1);
    return 1;
}
static auto each_descendant(lua_State* L) -> int {
    fs::path path = luaL_checkstring(L, 1);
    if (not fs::is_directory(path)) luaL_errorL(L, "not a directory '%s'", lua_tostring(L, 1));
    using Iter = fs::recursive_directory_iterator;
    dluau::make_userdata<Iter>(L, Iter(path));
    //lu::newuserdata<Iter>(L) = Iter(path);
    lua_pushcclosure(L, fs_iterator<Iter>, "recursive_directory_iterator", 1);
    return 1;
}
static auto relative(lua_State* L) -> int {
    fs::path path = luaL_checkstring(L, 1);
    fs::path base = luaL_optstring(L, 2, "");
    if (base.empty()) base = fs::current_path();
    lua_pushstring(L, fs::relative(path, base).string().c_str());
    return 1;
}
static auto proximate(lua_State* L) -> int {
    fs::path path = luaL_checkstring(L, 1);
    fs::path base = luaL_optstring(L, 2, "");
    if (base.empty()) base = fs::current_path();
    dluau::push(L, fs::proximate(path, base).string());
    return 1;
}
static auto temp_folder(lua_State* L) -> int {
    T_directory::create(L, fs::temp_directory_path());
    return 1;
}
static auto find_environment_variable(lua_State* L) -> int {
    if (const char* var = std::getenv(luaL_checkstring(L, 1))) {
        dluau::push(L, var);
        return 1;
    }
    return 0;
}
static auto exists(lua_State* L) -> int {
    dluau::push(L, fs::exists(luaL_checkstring(L, 1)));
    return 1;
}
static auto remove(lua_State* L) -> int {
    fs::path p = dluau::tostring(L, 1);
    std::error_code err{};
    if (!fs::remove(p, err)) dluau::arg_error(L, 1, err.message());
    return 0;
}
static auto remove_all(lua_State* L) -> int {
    fs::path p = dluau::tostring(L, 1);
    std::error_code err{};
    if (!fs::remove_all(p, err)) dluau::arg_error(L, 1, err.message());
    return 0;
}
static auto open_file(lua_State* L) -> int {
    T_file::create(L, luaL_checkstring(L, 1));
    return 1;
}
static auto open_directory(lua_State* L) -> int {
    T_directory::create(L, luaL_checkstring(L, 1));
    return 1;
}
static auto make_directory(lua_State* L) -> int {
    fs::path p = luaL_checkstring(L, 1);
    std::error_code err{};
    if (!fs::create_directory(p, err)) {
        if (!err) dluau::arg_error(L, 1, "directory already exists");
        else dluau::arg_error(L, 1, err.message());
    }
    T_directory::make(L, p);
    return 1;
}
static auto make_symlink(lua_State* L) -> int {
    const fs::path to = dluau::tostring(L, 1);
    const fs::path new_symlink = dluau::tostring(L, 2);
    std::error_code err{};
    if (fs::is_directory(to)) fs::create_directory_symlink(to, new_symlink, err);
    else fs::create_symlink(to, new_symlink, err);
    if (err) dluau::error(L, err.message());
    T_symlink::make(L, new_symlink);
    return 1;
}
static auto make_file(lua_State* L) -> int {
    fs::path p = luaL_checkstring(L, 1);
    create_new_file(L, p);
    return 1;
}
static auto copy(lua_State* L) -> int {
    const fs::path from = dluau::tostring(L, 1);
    const fs::path to = dluau::tostring(L, 2);
    std::error_code err{};
    const int top = lua_gettop(L);
    if (top > 2) {
        fs::copy_options copts{};
        for (int i{3}; i <= top; ++i) {
            copts |= to_copy_options(luaL_checkstring(L, i));
        }
        fs::copy(from, to, copts, err);
    } else {
        fs::copy(from, to, err);
    }
    if (err) dluau::error(L, err.message());
    if (fs::is_directory(to)) {
        T_directory::make(L, to);
        return 1;
    } else if (fs::is_regular_file(to)) {
        T_file::make(L, to);
        return 1;
    }
    return 0;
}
DLUAUSTD_API auto dlrequire(lua_State* L) -> int {
    const luaL_Reg lib[] = {
        {"current_directory", current_directory},
        {"temp_directory", temp_folder},
        {"find_environment_variable", find_environment_variable},
        {"exists", exists},
        {"canonical", canonical},
        {"weakly_canonical", weakly_canonical},
        {"relative", relative},
        {"proximate", proximate},
        {"absolute", absolute},
        {"each_child", each_child},
        {"each_descendant", each_descendant},
        {"make_directory", make_directory},
        {"open_directory", open_directory},
        {"make_file", make_file},
        {"make_symlink", make_symlink},
        {"open_file", open_file},
        {"copy", copy},
        {"remove", remove},
        {"remove_all", remove_all},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    return 1;
}
