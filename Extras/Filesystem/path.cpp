#include "module.hpp"
#include <lualib.h>
#include <lumin.h>
#include <filesystem>
#include <unordered_map>
namespace fs = std::filesystem;
using namecall_function = int(*)(lua_State*, Path&);
static std::unordered_map<int, namecall_function> namecalls{};
static int tag = lumin_newuserdatatag();

static int div(lua_State* L) {
    if (lua_isstring(L, 1)) {
        Path& self = *static_cast<Path*>(lua_touserdatatagged(L, 2, tag));
        return create_path(L, lua_tostring(L, 1) / self);
    }
    Path& self = *static_cast<Path*>(lua_touserdatatagged(L, 1, tag));
    if (lua_userdatatag(L, 1) == lua_userdatatag(L, 2)) {
        return create_path(L, self / *static_cast<Path*>(lua_touserdatatagged(L, 2, tag)));
    } else if (lua_isstring(L, 2)) {
        return create_path(L, self / lua_tostring(L, 2));
    }
    luaL_typeerrorL(L, 2, "Path");
}
static int absolute(lua_State* L, Path& self) {
    return create_path(L, fs::absolute(self));
}
static int namecall(lua_State* L) {
    Path& self = *static_cast<Path*>(lua_touserdatatagged(L, 1, tag));
    int atom;
    lua_namecallatom(L, &atom);
    auto found_it = namecalls.find(atom);
    if (found_it == namecalls.end()) luaL_errorL(L, "invalid namecall");
    return found_it->second(L, self);
}
static int tostring(lua_State* L) {
    Path& self = *static_cast<Path*>(lua_touserdatatagged(L, 1, tag));
    lua_pushstring(L, self.string().c_str());
    return 1;
}
static const luaL_Reg meta[] = {
    {"__namecall", namecall},
    {"__tostring", tostring},
    {"__div", div},
    {nullptr, nullptr}
};
static int register_namecall(lua_State* L, std::string_view key, namecall_function call) {
    lua_pushlstring(L, key.data(), key.size());
    int atom;
    lua_tostringatom(L, -1, &atom);
    lua_pop(L, 1);
    namecalls.emplace(atom, call);
    return atom;
}

int create_path(lua_State* L, const std::filesystem::path& p) {
    Path* self = static_cast<Path*>(lua_newuserdatatagged(L, sizeof(Path), tag));
    luaL_getmetatable(L, path_tname);
    lua_setmetatable(L, -2);
    new (self) Path{p};
    return 1;
}
int path_ctor(lua_State* L) {
    return create_path(L, luaL_checkstring(L, 1));
}
void init_path(lua_State* L) {
    tag = 1;
    if (luaL_newmetatable(L, path_tname)) {
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, path_tname);
        lua_setfield(L, -2, "__type");
        register_namecall(L, "parent_path", [](lua_State* L, Path& self) -> int {
            return create_path(L, self.parent_path());
        });
        register_namecall(L, "absolute", [](lua_State* L, Path& self) -> int {
            return create_path(L, fs::absolute(self));
        });
        register_namecall(L, "extension", [](lua_State* L, Path& self) -> int {
            lua_pushstring(L, self.extension().string().c_str());
            return 1;
        });
        register_namecall(L, "filename", [](lua_State* L, Path& self) -> int {
            lua_pushstring(L, self.filename().string().c_str());
            return 1;
        });
        register_namecall(L, "has_extension", [](lua_State* L, Path& self) -> int {
            lua_pushboolean(L, self.has_extension());
            return 1;
        });
        register_namecall(L, "has_filename", [](lua_State* L, Path& self) -> int {
            lua_pushboolean(L, self.has_filename());
            return 1;
        });
        register_namecall(L, "stem", [](lua_State* L, Path& self) -> int {
            lua_pushstring(L, self.stem().string().c_str());
            return 1;
        });
        register_namecall(L, "has_stem", [](lua_State* L, Path& self) -> int {
            lua_pushboolean(L, self.has_stem());
            return 1;
        });
        register_namecall(L, "replace_extension", [](lua_State* L, Path& self) -> int {
            return create_path(L, self.replace_extension(luaL_checkstring(L, 2)));
        });
        register_namecall(L, "replace_filename", [](lua_State* L, Path& self) -> int {
            return create_path(L, self.replace_filename(luaL_checkstring(L, 2)));
        });
        register_namecall(L, "exists", [](lua_State* L, Path& self) -> int {
            lua_pushboolean(L, fs::exists(self));
            return 1;
        });
        //lua_setuserdatametatable(L, path_tag, -1);
        lua_setuserdatadtor(L, tag, [](lua_State* L, void* data) -> void {
            static_cast<Path*>(data)->~Path();
        });
    }
    lua_pop(L, 1);
}
