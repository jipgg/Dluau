#include <dluau.h>
#include "shared.hpp"
#include <format>
#include <functional>
#include <boost/container/flat_map.hpp>
using std::string;
using std::string_view;
using std::cout, std::format;
using std::function, std::bind;
using boost::container::flat_map;
static string text_color_cache{"default"};
static string background_color_cache{"default"};
static auto set = [](int v) {
    cout << format("\033[{}m", v);
};
static flat_map<string_view, function<void(int)>> color_setters = {
    {"black", [](int off) {set(off + 30);}},
    {"red", [](int off) {set(off + 31);}},
    {"green", [](int off) {set(off + 32);}},
    {"yellow", [](int off) {set(off + 33);}},
    {"blue", [](int off) {set(off + 34);}},
    {"magenta", [](int off) {set(off + 35);}},
    {"cyan", [](int off) {set(off + 36);}},
    {"light gray", [](int off) {set(off + 37);}},
    {"default", [](int off) {set(off + 39);}},
    {"gray", [](int off) {set(off + 90);}},
    {"bright red", [](int off) {set(off + 91);}},
    {"bright green", [](int off) {set(off + 92);}},
    {"bright yellow", [](int off) {set(off + 93);}},
    {"bright blue", [](int off) {set(off + 94);}},
    {"bright magenta", [](int off) {set(off + 95);}},
    {"bright cyan", [](int off) {set(off + 96);}},
    {"white", [](int off) {set(off + 97);}},
};

static bool set_color(string_view col, bool bg = false) {
    int off = bg ? 10 : 0;
    if (not color_setters.contains(col)) return false;
    color_setters[col](off);
    if (bg) background_color_cache = col;
    else text_color_cache = col;
    return true;
}

static string get_print_statement(lua_State* L, int offset = 0) {
    const int top = lua_gettop(L);
    string to_print;
    for (int i{1 + offset}; i <= top; ++i) {
        size_t len{};
        std::string_view v{luaL_tolstring(L, i, &len), len};
        to_print.append(v).append(", ");
    }
    to_print.resize(to_print.size() - 2);
    return to_print;
}

static int call(lua_State* L) {
    std::cout << get_print_statement(L, 1) << '\n'; 
    return 0;
}
static int index(lua_State* L) {
    string_view key = luaL_checkstring(L, 2);
    switch(key.at(0)) {
        case 't':
            lua_pushstring(L, text_color_cache.c_str());
            return 1;
        case 'b':
            lua_pushstring(L, background_color_cache.c_str());
            return 1;
    }
    luaL_argerrorL(L, 2, "unknown field");
}
static int newindex(lua_State* L) {
    string_view key = luaL_checkstring(L, 2);
    string_view v = luaL_checkstring(L, 3);
    switch(key.at(0)) {
        case 't':
            if (set_color(v, false)) return 0;
            break;
        case 'b':
            if (set_color(v, true)) return 0;
            break;
    }
    luaL_argerrorL(L, 2, "unknown field");
}
void dluauopen_print(lua_State *L) {
    const luaL_Reg funcs[] = {
        {"reset_colors", [](lua_State* L) {
            std::cout << "\033[0m";
            background_color_cache = "default";
            text_color_cache = "default";
            return 0;
        }},
        {"write", [](lua_State* L) {
            std::cout << get_print_statement(L);
            return 0;
        }},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, funcs);
    lua_newtable(L);
    const luaL_Reg meta[] = {
        {"__call", call},
        {"__index", index},
        {"__newindex", newindex},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, meta);
    lua_setmetatable(L, -2);
    lua_setglobal(L, "print");
}
