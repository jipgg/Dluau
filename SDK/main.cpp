#include <Windows.h>
#include <lualib.h>
#include <luacode.h>
#include "luminutils.h"
#include <ranges>
#include <lumin.h>
#include <format>
#include <iostream>

static int run(const std::string& source) {
    lua_State* L = lumin_initstate();
    if (const char* err = luminU_spawnscript(L, source.c_str())) {
        std::cerr << std::format("\033[31m{}\033[0m\n", err);
        return -1;
    }
    return 0;
}
int lumin_main(std::span<std::string_view> args) {
    for (auto e : args) {
        std::cout << std::format("> {}\n", e);
    }
    if (args[0] == "run") return run(std::string(args[1]));
    return 0;
}
