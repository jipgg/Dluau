#include <lumin.h>
#include <lualib.h>
#include <luminutils.h>
#include <Error_info.hpp>
#include <core.hpp>
#include <lib.hpp>
#include <ranges>
#include <optional>

int lumin_stringatom(lua_State* L, const char *key) {
    lua_pushstring(L, key);
    int atom;
    lua_tostringatom(L, -1, &atom);
    lua_pop(L, 1);
    return atom;
}
int lumin_lstringatom(lua_State* L, const char *key, size_t len) {
    lua_pushlstring(L, key, len);
    int atom;
    lua_tostringatom(L, -1, &atom);
    lua_pop(L, 1);
    return atom;
}
lua_State* lumin_initstate() {
    lua_State* L = luaL_newstate();
    lua_callbacks(L)->useratom = lumin_useratom;
    luaL_openlibs(L);
    lumin_loadfuncs(L);
    luminopen_dll(L);
    luminopen_fs(L);
    luminopen_process(L);
    const luaL_Reg script_meta[] = {
        {"__index", [](lua_State* L) -> int {
            const std::string_view key = luaL_checkstring(L, 2);
            if (key == "file") {
                if (not script_path_registry.contains(L)) luaL_errorL(L, "unexpected error. script doesn't have a path");
                libfs::new_file(L, {.path = script_path_registry[L]});
                return 1;
            }
            luaL_errorL(L, "invalid index");
        }},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    lua_newtable(L);
    luaL_register(L, nullptr, script_meta);
    lua_setmetatable(L, -2);
    lua_setglobal(L, "script");
    return L;
}
void lumin_init(lua_State* L) {
    lua_callbacks(L)->useratom = lumin_useratom;
    luaL_openlibs(L);
    lumin_loadfuncs(L);
    luminopen_dll(L);
    luminopen_fs(L);
    luminopen_process(L);
}
const char* lumin_run(lumin_Run_options opts) {
    if (opts.args) process_args = opts.args;
    lua_State* L = lumin_initstate();
    const char* errmsg{};
    for (auto sr : std::views::split(std::string_view(opts.scripts), ' ')) {
        if (auto* err = luminU_spawnscript(L, sr.data(), sr.size())) {
            errmsg = err;
        }
    }
    return errmsg ? errmsg : nullptr;
}
