#include ".luauxtm_api.h"
#include <luauxt.h>
#include <lualib.h>
#include <iostream>
#include <format>
#include <unordered_map>
static void register_ansi_code(lua_State* L, const char* field, const char* code) {
    lua_pushstring(L, code);
    lua_setfield(L, -2, field);
}
static int lua_ansi(lua_State* L) {
    std::cout << std::string("\033") + luaL_checkstring(L, 1);
    return 0;
}
static int lua_clear(lua_State* L) {
    std::cout << "\033[2J\033[H\033[0m";
    return 0;
}
static int lua_read(lua_State* L) {
    std::string input;
    std::cin >> input;
    lua_pushstring(L, input.c_str());
    return 1;
}
static int lua_write(lua_State* L) {
    std::cout << lua_tostring(L, 1);
    return 0;
}
static int lua_errwrite(lua_State* L) {
    std::cerr << lua_tostring(L, 1);
    return 0;
}
static int lua_cursor_moveup(lua_State* L) {
    std::cout << std::format("\033[{}A", luaL_optinteger(L, 1, 1));
    return 0;
}
static int lua_cursor_move_down(lua_State* L) {
    std::cout << std::format("\033[{}B", luaL_optinteger(L, 1, 1));
    return 0;
}
static int lua_cursor_moveright(lua_State* L) {
    std::cout << std::format("\033[{}C", luaL_optinteger(L, 1, 1));
    return 0;
}
static int lua_cursor_moveleft(lua_State* L) {
    std::cout << std::format("\033[{}D", luaL_optinteger(L, 1, 1));
    return 0;
}
static const luaL_Reg cursor[] = {
    {"move_up", lua_cursor_moveup},
    {"move_down", lua_cursor_move_down},
    {"move_right", lua_cursor_moveright},
    {"move_left", lua_cursor_moveleft},
    {nullptr, nullptr}
};
static const luaL_Reg library[] = {
    {"ansi", lua_ansi},
    {"read", lua_read},
    {"write", lua_write},
    {"errwrite", lua_errwrite},
    {"clear", lua_clear},
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
LUAUXTM_API inline void print_version() {
    std::cout << std::format("VERSION: {}\n", 123);
}
LUAUXTM_API inline int get_version() {
    return 1234;
}
LUAUXTM_API inline int add_numbers(double a, double b) {
    return static_cast<int>(a + b);
}
LUAUXTM_API inline int print_string(const char* string) {
    std::cout << std::format("STRING IS: {}\n", string);
    return 0;
}
LUAUXTM_API inline int luauxtm_load(lua_State* L) {
    lua_newtable(L);
    luaL_register(L, nullptr, library);
    lua_newtable(L);
    lua_newtable(L);
    push_ansi_codes(L, fg_codes);
    lua_setreadonly(L, -1, true);
    lua_setfield(L, -2, "fg");
    lua_newtable(L);
    push_ansi_codes(L, bg_codes);
    lua_setreadonly(L, -1, true);
    lua_setfield(L, -2, "bg");
    lua_pushstring(L, "\033[0m");
    lua_setfield(L, -2, "reset");
    lua_setfield(L, -2, "codes");
    lua_newtable(L);
    luaL_register(L, nullptr, cursor);
    lua_setfield(L, -2, "cursor");
    return 1;
}
