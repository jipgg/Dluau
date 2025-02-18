#include <dluau.h>
#include <dluau.hpp>
#include <iostream>
using namespace dluau::type_aliases;
static String separator{"\t"};

static String get_print_statement(lua_State* L, int offset = 0) {
    const int top = lua_gettop(L);
    String to_print;
    for (int i{1 + offset}; i <= top; ++i) {
        size_t len{};
        Strview v{luaL_tolstring(L, i, &len), len};
        to_print.append(v).append(separator);
    }
    to_print.resize(to_print.size() - separator.size());
    return to_print;
}

static int call(lua_State* L) {
    std::cout << get_print_statement(L, 1) << '\n'; 
    return 0;
}
static int index(lua_State* L) {
    Strview key = luaL_checkstring(L, 2);
    switch(key.at(0)) {
        case 's':
            lua_pushlstring(L, separator.data(), separator.length());
            return 1;
    }
    luaL_argerrorL(L, 2, "unknown field");
}
static int newindex(lua_State* L) {
    Strview key = luaL_checkstring(L, 2);
    Strview v = luaL_checkstring(L, 3);
    switch(key.at(0)) {
        case 's':
            separator = v;
            return 0;
    }
    luaL_argerrorL(L, 2, "unknown field");
}
void dluauopen_print(lua_State *L) {
    const luaL_Reg funcs[] = {
        {"noline", [](lua_State* L) {
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
