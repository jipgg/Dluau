#include "dluau.h"
#include "shared.hpp"
#include <iostream>
using std::string;

static int call(lua_State* L) {
    string input;
    std::cin >> input;
    lua_pushlstring(L, input.data(), input.size());
    return 1;
}
static int buffer(lua_State* L) {
    size_t len{};
    char* buf{};
    if (lua_isbuffer(L, 1)) {
        buf = static_cast<char*>(lua_tobuffer(L, 1, &len));
        const int size = luaL_optinteger(L, 2, len);
        if (size > len) luaL_argerrorL(L, 2, "out of bounds");
        len = size;
    } else {
        len = luaL_checkinteger(L, 1);
        buf = static_cast<char*>(lua_newbuffer(L, len));
    }
    std::cin.read(buf, len);
    return 1;
}
static int number(lua_State* L) {
    double d; 
    std::cin >> d;
    lua_pushnumber(L, d);
    return 1;
}

int shared::push_scan(lua_State *L) {
    const luaL_Reg funcs[] = {
        {"buffer", buffer},
        {"number", number},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, funcs);
    lua_newtable(L);
    lua_pushcfunction(L, call, "scan.__call");
    lua_setfield(L, -2, "__call");
    lua_setmetatable(L, -2);
    return 1;
}
