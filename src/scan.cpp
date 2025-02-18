#include "dluau.h"
#include "dluau.hpp"
#include <iostream>
using std::string;

static auto call(lua_State* L) -> int {
    string input;
    std::cin >> input;
    lua_pushlstring(L, input.data(), input.size());
    return 1;
}
static auto buffer(lua_State* L) -> int {
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

void dluauopen_scan(lua_State *L) {
    const luaL_Reg funcs[] = {
        {"buffer", buffer},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, funcs);
    lua_newtable(L);
    lua_pushcfunction(L, call, "__call");
    lua_setfield(L, -2, "__call");
    lua_setmetatable(L, -2);
    lua_setglobal(L, "scan");
}

