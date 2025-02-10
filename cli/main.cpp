#include <Windows.h>
#include <lualib.h>
#include <luacode.h>
#include <ranges>
#include <dluau.h>
#include <format>
#include "cli.hpp"
#include <iostream>
#include <print>
using cli::Configuration;

static void output_error(Strview msg) {
    std::println(std::cerr, "\033[31m{}\033[0m", msg);
}
static int get_optimization_level(Span<char*> args) {
    constexpr Strview upper = "-O";
    auto found = ranges::find_if(args, [&upper](char* e) {
        constexpr Strview lower = "-o";
        Strview sv{e};
        sv = sv.substr(0, upper.size());
        return sv == upper or sv == lower;
    });
    if (found == ranges::end(args)) return 0;
    const String str = String(*found).substr(upper.size());
    return std::stoi(str);
}
static int get_debug_level(Span<char*> args) {
    constexpr Strview upper = "-D";
    auto found = ranges::find_if(args, [&upper](char* e) {
        constexpr Strview lower = "-d";
        Strview sv{e};
        sv = sv.substr(0, upper.size());
        return sv == upper or sv == lower;
    });
    if (found == ranges::end(args)) return 1;
    const String str = String(*found).substr(upper.size());
    return std::stoi(str);
}

static void expand_source_file(Strview file, String& to) {
    to.append(file) += ',';
}

static void expand_source_directory(Strview dir, String& to) {
    for (auto entry : fs::directory_iterator(dir)) {
        if (not entry.is_regular_file()) continue;
        const auto file = entry.path();
        if (file.extension() != ".luau") continue;
        expand_source_file(file.string(), to);
    }
}
static bool is_file_or_directory(Strview arg) {
    return arg.ends_with('/') or arg.ends_with(".luau");
}

static int without_command(Span<char*> arg_span) {
    String sources;
    const auto filter_sources = views::transform([](char* e) -> Strview {return e;})
        | views::filter(is_file_or_directory);
    for (auto arg : arg_span | filter_sources) {
        if (arg.ends_with('/')) expand_source_directory(arg, sources);
        else expand_source_file(arg, sources);
    }
    sources.pop_back();
    Configuration config{
        .sources = std::move(sources),
        .optimization_level = get_optimization_level(arg_span),
        .debug_level = get_debug_level(arg_span),
    };
    String args;
    for (char* v : arg_span) args.append(v) += ',';
    args.pop_back();
    return run(config, &args);
}
int main(int argc, char** argv) {
    try {
        Span<char*> arg_span{argv, static_cast<size_t>(argc)};
        if (arg_span.size() <= 1) {
            output_error("no input files given.");
            return -1;
        }
        Strview first_arg{arg_span[1]};
        if (is_file_or_directory(first_arg)) return without_command(arg_span);
        std::println(std::cerr, "\033[31mUnknown command '{}'.\033[0m", arg_span[1]);
        return -1;
    } catch (std::exception& e) {
        std::println(std::cerr, "\033[31mCaughr exception '{}'.\033[0m", e.what());
        return -1;
    }
}
