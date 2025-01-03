#include "api.hpp"
#include <lualib.h>
#include <filesystem>
#include <iostream>
using std::filesystem::path;
namespace fs = std::filesystem;
constexpr int tag{2};
constexpr int sentinel = -1;
namespace name {
static int hello = sentinel;
static int absolute = sentinel;
static int parent_path = sentinel;
}
static int create_path(lua_State* L, const path& p);

static int namecall(lua_State* L) {
    path& self = *static_cast<path*>(lua_touserdatatagged(L, 1, tag));
    int atom;
    lua_namecallatom(L, &atom);
    if (atom == name::hello) {
        std::cout << "HELLO WORLD\n";
        return 0;
    } else if (atom == name::parent_path) {
        return create_path(L, self.parent_path());
    } else if (atom == name::absolute) {
        return create_path(L, fs::absolute(self));
    }
    return 0;
}
static int tostring(lua_State* L) {
    path& self = *static_cast<path*>(lua_touserdatatagged(L, 1, tag));
    lua_pushstring(L, self.string().c_str());
    return 1;
}
static const luaL_Reg meta[] = {
    {"__namecall", namecall},
    {"__tostring", tostring},
    {nullptr, nullptr}
};
static int register_namecall(lua_State* L, std::string_view key) {
    lua_pushlstring(L, key.data(), key.size());
    int atom;
    std::cout << "namecall registered " << lua_tostringatom(L, -1, &atom) << "(" << atom << ")\n";
    lua_pop(L, 1);
    return atom;
}
static int create_path(lua_State* L, const path& p) {
    if (luaL_newmetatable(L, typeid(path).name())) {
        constexpr const char* type_name = "Path";
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, type_name);
        lua_setfield(L, -2, "__type");
        lua_setuserdatadtor(L, tag, [](lua_State* L, void* data) -> void {
            static_cast<path*>(data)->~path();
        });
        name::hello = register_namecall(L, "hello");
        name::absolute = register_namecall(L, "absolute");
        name::parent_path = register_namecall(L, "parent_path");
        std::cout << " INITIALIZED METATABLE FOR PATH\n";
    }
    lua_pop(L, 1);
    path* self = static_cast<path*>(lua_newuserdatatagged(L, sizeof(path), tag));
    luaL_getmetatable(L, typeid(path).name());
    lua_setmetatable(L, -2);
    new (self) path{p};
    return 1;
}

HALUA_API int Path(lua_State* L) {
    return create_path(L, luaL_checkstring(L, 1));
}
