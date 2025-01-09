#include "minluau_modules_api.h"
#include <minluau.h>
#include <lualib.h>
#include <iostream>
#include <optional>
#include "Windows.h"
#include <unordered_map>
static void register_ansi_code(lua_State* L, const char* field, const char* code) {
    lua_pushstring(L, code);
    lua_setfield(L, -2, field);
}
static int ansi_code(lua_State* L) {
    std::cout << std::string("\033") + luaL_checkstring(L, 1);
    return 0;
}
static const luaL_Reg library[] = {
    {"ansi_code", ansi_code},
    {nullptr, nullptr}
};

using ansi_code_map = std::unordered_map<std::string, std::string>;
static const ansi_code_map fg_codes{
    {"default", "\033[39m"},
    {"black", "\033[30m"},
    {"red", "\033[31m"},
    {"green", "\033[32m"},
    {"yellow", "\033[33m"},
    {"blue", "\033[34m"},
    {"magenta", "\033[35m"},
    {"cyan", "\033[36m"},
    {"white", "\033[37m"},
    {"gray", "\033[90m"},
    {"light_red", "\033[91m"},
    {"light_green", "\033[92m"},
    {"light_yellow", "\033[93m"},
    {"light_blue", "\033[94m"},
    {"light_magenta", "\033[95m"},
    {"light_cyan", "\033[96m"},
    {"bright_white", "\034[97m"},
};
static const ansi_code_map bg_codes{
    {"default", "\033[49m"},
    {"black", "\033[40m"},
    {"red", "\033[41m"},
    {"green", "\033[42m"},
    {"yellow", "\033[43m"},
    {"blue", "\033[44m"},
    {"magenta", "\033[45m"},
    {"cyan", "\033[46m"},
    {"white", "\033[47m"},
    {"gray", "\033[100m"},
    {"light_red", "\033[101m"},
    {"light_green", "\033[102m"},
    {"light_yellow", "\033[103m"},
    {"light_blue", "\033[104m"},
    {"light_magenta", "\033[105m"},
    {"light_cyan", "\033[106m"},
    {"bright_white", "\033[107m"},
};
static void push_ansi_codes(lua_State* L, const ansi_code_map& map) {
    for (auto [key, val] : map) {
        lua_pushstring(L, val.c_str());
        lua_setfield(L, -2, key.c_str());
    }
}
MINLUAU_MODULES_API inline int import_console(lua_State* L) {
    lua_newtable(L);
    luaL_register(L, nullptr, library);
    lua_newtable(L);
    lua_newtable(L);
    push_ansi_codes(L, fg_codes);
    lua_setfield(L, -2, "fg");
    lua_newtable(L);
    push_ansi_codes(L, bg_codes);
    lua_setfield(L, -2, "bg");
    lua_pushstring(L, "\033[0m");
    lua_setfield(L, -2, "reset");
    lua_setfield(L, -2, "codes");
    return 1;
}
