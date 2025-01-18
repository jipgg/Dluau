#include <Windows.h>
#include <lualib.h>
#include <luacode.h>
#include "luminutils.h"
#include <ranges>
#include <lumin.h>
#include <format>
#include <iostream>
#include <vector>

static int run(const std::string& source, std::span<std::string_view> args) {
    const char* data = source.c_str();
    std::string argdata;
    for (auto arg : args) {
        argdata += arg;
        argdata += ' ';
    }
    argdata.pop_back();
    auto err = lumin_run({
        .scripts = source.c_str(),
        .args = argdata.c_str(),
    });
    if (err) {
        std::cerr << std::format("\033[31m{}\033[0m\n", err);
        return -1;
    }
    return 0;
}
int lumin_main(std::span<std::string_view> args) {
    if (args[0] == "run") return run(std::string(args[1]), args.subspan(2));
    return 0;
}
