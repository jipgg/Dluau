#include "api.hpp"
#include <lualib.h>
#include <iostream>
enum class Color {
    Default, Black, Red, Green, Yellow, Blue, Magenta, Cyan,
    LightGray, DarkGray, LightRed, LightGreen, LightYellow, LightBlue,
    LightMagenta, LightCyan, White,
};

static int set_foreground_color(lua_State* L) {
    const int color = luaL_checkinteger(L, 1);
    if (color < static_cast<int>(Color::Default) or color > static_cast<int>(Color::White)) {
        luaL_argerrorL(L, 1, "out of range");
    }
    using std::cout;
    switch (static_cast<Color>(color)) {
        case Color::Default: cout << "\033[39m"; break;
        case Color::Black: cout << "\033[30m"; break;
        case Color::Red: cout << "\033[31m"; break;
        case Color::Green: cout << "\033[32m"; break;
        case Color::Yellow: cout << "\033[33m"; break;
        case Color::Blue: cout << "\033[34m"; break;
        case Color::Magenta: cout << "\033[35m"; break;
        case Color::Cyan: cout << "\033[36m"; break;
        case Color::LightGray: cout << "\033[37m"; break;
        case Color::DarkGray: cout << "\033[90m"; break;
        case Color::LightRed: cout << "\033[91m"; break;
        case Color::LightGreen: cout << "\033[92m"; break;
        case Color::LightYellow: cout << "\033[93m"; break;
        case Color::LightBlue: cout << "\033[94m"; break;
        case Color::LightMagenta: cout << "\033[95m"; break;
        case Color::LightCyan: cout << "\033[96m"; break;
        case Color::White: cout << "\033[97m"; break;
    }
    return 0;
}
static int ansi(lua_State* L) {
    std::cout << std::string("\033") + luaL_checkstring(L, 1);
    return 0;
}
static int set_background_color(lua_State* L) {
    const int color = luaL_checkinteger(L, 1);
    if (color < static_cast<int>(Color::Default) or color > static_cast<int>(Color::White)) {
        luaL_argerrorL(L, 1, "out of range");
    }
    using std::cout;
    switch (static_cast<Color>(color)) {
        case Color::Default: cout << "\033[49m"; break;
        case Color::Black: cout << "\033[40m"; break;
        case Color::Red: cout << "\033[41m"; break;
        case Color::Green: cout << "\033[42m"; break;
        case Color::Yellow: cout << "\033[43m"; break;
        case Color::Blue: cout << "\033[44m"; break;
        case Color::Magenta: cout << "\033[45m"; break;
        case Color::Cyan: cout << "\033[46m"; break;
        case Color::LightGray: cout << "\033[47m"; break;
        case Color::DarkGray: cout << "\033[100m"; break;
        case Color::LightRed: cout << "\033[101m"; break;
        case Color::LightGreen: cout << "\033[102m"; break;
        case Color::LightYellow: cout << "\033[103m"; break;
        case Color::LightBlue: cout << "\033[104m"; break;
        case Color::LightMagenta: cout << "\033[105m"; break;
        case Color::LightCyan: cout << "\033[106m"; break;
        case Color::White: cout << "\033[107m"; break;
    }
    return 0;
}

static void color_table(lua_State* L) {
    auto value = [&L](const char* field, Color color) {
        lua_pushinteger(L, static_cast<int>(color));
        lua_setfield(L, -2, field);
    };
    lua_newtable(L);
    value("Default", Color::Default);
    value("Black", Color::Black);
    value("Red", Color::Red);
    value("Green", Color::Green);
    value("Yellow", Color::Yellow);
    value("Blue", Color::Blue);
    value("Magenta", Color::Magenta);
    value("Cyan", Color::Cyan);
    value("LightGray", Color::LightGray);
    value("DarkGray", Color::DarkGray);
    value("LightRed", Color::LightRed);
    value("LightGreen", Color::LightGreen);
    value("LightYellow", Color::LightYellow);
    value("LightBlue", Color::LightBlue);
    value("LightMagenta", Color::LightMagenta);
    value("LightCyan", Color::LightCyan);
    value("White", Color::White);
}
static const luaL_Reg library[] = {
    {"set_foreground_color", set_foreground_color},
    {"set_background_color", set_background_color},
    {"ansi", ansi},
    {nullptr, nullptr}
};
HALUA_API int stdconsole(lua_State* L) {
    lua_newtable(L);
    luaL_register(L, nullptr, library);
    color_table(L);
    lua_setfield(L, -2, "Color");
    return 1;
}
