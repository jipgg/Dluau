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
using namespace dluau::type_aliases;
static void output_error(Strview msg) {
    std::print(std::cerr, "\033[31m{}\033[0m\n", msg);
}

static Opt<String> get_if_param(Span<const Strview> argv, Strview prefix) {
    auto found = ranges::find_if(argv, [&prefix](Strview e) {
        return e.substr(0, prefix.length()) == prefix;
    });
    if (found == ranges::end(argv)) return std::nullopt;
    return std::make_optional<String>(found->substr(prefix.length()));
}
static bool contains(Span<const Strview> argv, Strview v) {
    return ranges::find_if(argv, [&v](Strview e) {return v == e;}) != ranges::end(argv);
}
int host_main(const Vector<Strview>& args) {
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
        std::cerr << std::format("Unexpected error: {}.\n", e.what());
        return -1;
    }
}
