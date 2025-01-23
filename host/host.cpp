#include <Windows.h>
#include <lualib.h>
#include <luacode.h>
#include <Error_info.hpp>
#include <ranges>
#include <luauxt.h>
#include <optional>
#include <format>
#include <iostream>
#include <misc.hpp>
#include <optional>
#include "host_main.hpp"
#include <ranges>
namespace rn = std::ranges;
static void output_error(std::string_view msg) {
    std::cerr << std::format("\033[31m{}\033[0m\n", msg);
}

static std::optional<std::string> get_if_param(std::span<const std::string_view> argv, std::string_view prefix) {
    auto found = rn::find_if(argv, [&prefix](std::string_view e) {
        return e.substr(0, prefix.length()) == prefix;
    });
    if (found == rn::end(argv)) return std::nullopt;
    return std::make_optional<std::string>(found->substr(prefix.length()));
}
static bool contains(std::span<const std::string_view> argv, std::string_view v) {
    return rn::find_if(argv, [&v](std::string_view e) {return v == e;}) != rn::end(argv);
}
int host_main(const std::vector<std::string_view>& args) {
    try {
        auto src = get_if_param(args, "--sources=");
        auto ar = get_if_param(args, "--args=");
        goluau_GoOptions opts{};
        opts.scripts = src ? src->c_str() : nullptr;
        opts.args = ar ? ar->c_str() : nullptr;
        opts.optimization_level = contains(args, "-O2") ? 2 : contains(args, "-O1") ? 1 : 0;
        opts.debug_level = contains(args, "-D0") ? 0 : 1;
        return luauxt_run(&opts);
    } catch (std::exception& e) {
        std::cerr << std::format("Unexpected error: {}.\n", e.what());
        return -1;
    }
}
