#include <luauxt.h>
#include <lualib.h>
#include <Error_info.hpp>
#include <core.hpp>
#include <filesystem>
#include <ranges>
#include <optional>

int luauxt_stringatom(lua_State* L, const char *key) {
    lua_pushstring(L, key);
    int atom;
    lua_tostringatom(L, -1, &atom);
    lua_pop(L, 1);
    return atom;
}
int luauxt_lstringatom(lua_State* L, const char *key, size_t len) {
    lua_pushlstring(L, key, len);
    int atom;
    lua_tostringatom(L, -1, &atom);
    lua_pop(L, 1);
    return atom;
}
lua_State* luauxt_newstate() {
    lua_State* L = luaL_newstate();
    lua_callbacks(L)->useratom = luauxt_defaultuseratom;
    luauxt_openlibs(L);
    const luaL_Reg funcs[] = {
        {"command_line_arguments", [](lua_State* L) -> int {
            if (launch_args.empty()) return 0;
            using std::views::split;
            lua_newtable(L);
            int i{1};
            for (auto sv : launch_args | split(arg_separator)) {
                lua_pushlstring(L, sv.data(), sv.size());
                lua_rawseti(L, -2, i++);
            }
            return 1;
        }},
        {"script_directory", [](lua_State* L) {
            if (not script_path_registry.contains(L)) luaL_errorL(L, "unexpected error. script doesn't have a path");
            const std::filesystem::path path{script_path_registry[L]}; 
            lua_pushstring(L, (path.parent_path().string().c_str()));
            return 1;
        }},
        {"script_name", [](lua_State* L) {
            if (not script_path_registry.contains(L)) luaL_errorL(L, "unexpected error. script doesn't have a path");
            const std::filesystem::path path{script_path_registry[L]}; 
            lua_pushstring(L, (path.filename().string().c_str()));
            return 1;
        }},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, funcs);
    lua_setglobal(L, "meta");
    return L;
}
void luauxt_init(lua_State* L) {
    lua_callbacks(L)->useratom = luauxt_defaultuseratom;
    luauxt_openlibs(L);
}
