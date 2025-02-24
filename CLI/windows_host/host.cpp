#include <Windows.h>
#include <lualib.h>
#include <luacode.h>
#include <ranges>
#include <dluau.hpp>
#include <optional>
#include <format>
#include <iostream>
#include <optional>
#include "host_main.hpp"
#include <print>
#include <ranges>
namespace rngs = std::ranges;
using std::string_view, std::string;
using std::optional;
using Args = std::vector<string_view>;
using Args_view = std::span<const string_view>;

static auto get_if_param(Args_view argv, string_view prefix) -> optional<string> {
    auto found = rngs::find_if(argv, [&prefix](string_view e) {
        return e.substr(0, prefix.length()) == prefix;
    });
    if (found == rngs::end(argv)) return std::nullopt;
    return std::make_optional<string>(found->substr(prefix.length()));
}
static auto contains(Args_view argv, string_view v) -> bool {
    return rngs::find_if(argv, [&v](auto e){return v==e;}) != rngs::end(argv);
}
auto host_main(const Args& args) -> int {
    try {
        auto src = get_if_param(args, "--sources=");
        auto ar = get_if_param(args, "--args=");
        dluau_InitOptions opts{};
        opts.scripts = src ? src->c_str() : nullptr;
        opts.args = ar ? ar->c_str() : nullptr;
        opts.optimization_level = contains(args, "-O2") ? 2 : contains(args, "-O1") ? 1 : 0;
        opts.debug_level = contains(args, "-D0") ? 0 : 1;
        return dluau_run(&opts);
    } catch (std::exception& e) {
        std::println(std::cerr, "Unexpected error: {}.", e.what());
        return -1;
    }
}
