#include <Windows.h>
#include <lualib.h>
#include <luacode.h>
#include <common.hpp>
#include <ranges>
#include <dluau.h>
#include <optional>
#include <format>
#include <iostream>
#include <optional>
#include "host_main.hpp"
#include <ranges>
namespace rn = std::ranges;
using std::string_view, std::span, std::vector, std::string;
using std::optional, std::make_optional;
using std::format;
static void output_error(string_view msg) {
    std::cerr << format("\033[31m{}\033[0m\n", msg);
}

static optional<string> get_if_param(span<const string_view> argv, string_view prefix) {
    auto found = rn::find_if(argv, [&prefix](string_view e) {
        return e.substr(0, prefix.length()) == prefix;
    });
    if (found == rn::end(argv)) return std::nullopt;
    return make_optional<string>(found->substr(prefix.length()));
}
static bool contains(span<const string_view> argv, string_view v) {
    return rn::find_if(argv, [&v](string_view e) {return v == e;}) != rn::end(argv);
}
int host_main(const vector<string_view>& args) {
    try {
        auto src = get_if_param(args, "--sources=");
        auto ar = get_if_param(args, "--args=");
        dluau_runoptions opts{};
        opts.scripts = src ? src->c_str() : nullptr;
        opts.args = ar ? ar->c_str() : nullptr;
        opts.optimization_level = contains(args, "-O2") ? 2 : contains(args, "-O1") ? 1 : 0;
        opts.debug_level = contains(args, "-D0") ? 0 : 1;
        return dluau_run(&opts);
    } catch (std::exception& e) {
        std::cerr << format("Unexpected error: {}.\n", e.what());
        return -1;
    }
}
