#include <dluau.h>
#include <shared.hpp>

consteval const char* get_platform() {
#if defined(_WIN32)
    return "Windows";
#elif defined(__linux__)
    return "Linux";
#elif defined(__APPLE__) && defined(__MACH__)
    return "macOS";
#endif
    return "unknown";
}

static int newindex(lua_State* L) {
    luaL_argerrorL(L, 2, "fields are readonly");
}
static int index(lua_State* L) {
    std::string_view key = luaL_checkstring(L, 2); 
    const auto& paths = dluau::get_script_paths();
    switch (key[0]) {
        case 'p': { /*platform*/
            lua_pushstring(L, get_platform());
            return 1;
        }
        case 'a': { /*args*/
            if (dluau::args.empty()) return 0;
            using std::views::split;
            lua_newtable(L);
            int i{1};
            for (auto sv : dluau::args | split(dluau::arg_separator)) {
                lua_pushlstring(L, sv.data(), sv.size());
                lua_rawseti(L, -2, i++);
            }
            return 1;
        }
        case 'l': { /*local_directory*/
            if (not paths.contains(L)) luaL_errorL(L, "unexpected error. script doesn't have a path");
            const std::filesystem::path path{paths.at(L)}; 
            lua_pushstring(L, (path.parent_path().string().c_str()));
            return 1;
        }
        case 'c': { /*current_script*/
            if (not paths.contains(L)) luaL_errorL(L, "unexpected error. script doesn't have a path");
            const std::filesystem::path path{paths.at(L)}; 
            lua_pushstring(L, (path.filename().string().c_str()));
            return 1;
        }
    }
    luaL_argerrorL(L, 2, "unknown field");
}
constexpr luaL_Reg metatable[] = {
    {"__index", index},
    {"__newindex", newindex},
    {nullptr, nullptr}
};
namespace dluau {
int push_metadatatable(lua_State* L) {
    lua_newtable(L);
    lua_newtable(L);
    luaL_register(L, nullptr, metatable);
    lua_pushstring(L, "locked");
    lua_setfield(L, -2, "__metatable");
    lua_setmetatable(L, -2);
    return 1;
}
}
