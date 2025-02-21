#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <print>
#include <span>
#include "io.hpp"

#include <iostream>
using std::string, std::string_view;

static string separator{"\t"};

static auto get_print_statement(lua_State* L, int offset = 0) -> string {
    const int top = lua_gettop(L);
    string to_print;
    for (int i{1 + offset}; i <= top; ++i) {
        size_t len{};
        string_view v{luaL_tolstring(L, i, &len), len};
        to_print.append(v).append(separator);
    }
    to_print.resize(to_print.size() - separator.size());
    return to_print;
}
static auto println(lua_State* L) -> int {
    std::println("{}", dluau::tostring(L, 1));
    return 0;
}
static auto print(lua_State* L) -> int {
    std::print("{}", dluau::tostring(L, 1));
    return 0;
}
static auto write(lua_State* L) -> int {
    size_t len;
    std::span buf{static_cast<char*>(luaL_checkbuffer(L, 1, &len)), len};
    std::cout.write(buf.data(), buf.size());
    return 0;
}
static auto scan(lua_State* L) -> int {
    std::string input;
    std::cin >> input;
    lua_pushstring(L, input.c_str());
    return 1;
}
static auto read(lua_State* L) {
    size_t len;
    std::span buf{static_cast<char*>(luaL_checkbuffer(L, 1, &len)), len};
    std::cin.read(buf.data(), buf.size());
    return 0;
}
static auto flush(lua_State* L) -> int {
    std::cout.flush();
    return 0;
}
static auto escape_code(lua_State* L) -> int {
    dluau::push(L, "\033{}", luaL_checkstring(L, 1));
    return 1;
}

DLUAUSTD_API auto dlrequire(lua_State* L) -> int {
    const luaL_Reg lib[] = {
        {"println", println},
        {"write", write},
        {"print", print},
        {"escape_code", escape_code},
        {"scan", scan},
        {"read", read},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    return 1;
}
