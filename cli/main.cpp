#include <Windows.h>
#include <lualib.h>
#include <luacode.h>
#include <ranges>
#include <dluau.h>
#include <format>
#include "cli.hpp"
#include <iostream>
#include <print>
namespace rngs = std::ranges;
namespace vws = std::views;
namespace fs = std::filesystem;
using cli::Configuration;
using std::string_view, std::string;
using std::span, std::println, std::cerr;
constexpr const char* red_fmt = "\033[31m{}\033[0m";

static auto get_optimization_level(span<char*> args) -> int {
    constexpr string_view upper = "-O";
    auto found = rngs::find_if(args, [&upper](char* e) {
        constexpr string_view lower = "-o";
        string_view sv{e};
        sv = sv.substr(0, upper.size());
        return sv == upper or sv == lower;
    });
    if (found == rngs::end(args)) return 0;
    const string str = string(*found).substr(upper.size());
    return std::stoi(str);
}
static auto get_debug_level(span<char*> args) -> int {
    constexpr string_view upper = "-D";
    auto found = rngs::find_if(args, [&upper](char* e) {
        constexpr string_view lower = "-d";
        string_view sv{e};
        sv = sv.substr(0, upper.size());
        return sv == upper or sv == lower;
    });
    if (found == rngs::end(args)) return 1;
    const string str = string(*found).substr(upper.size());
    return std::stoi(str);
}

static void expand_source_file(string_view file, string& to) {
    to.append(file) += ',';
}

static void expand_source_directory(string_view dir, string& to) {
    for (auto entry : fs::directory_iterator(dir)) {
        if (not entry.is_regular_file()) continue;
        const auto file = entry.path();
        if (file.extension() != ".luau") continue;
        expand_source_file(file.string(), to);
    }
}
static auto is_source(string_view e) -> bool {
    return e.ends_with('/') or e.ends_with(".luau");
};
static auto without_command(span<char*> arg_span) -> int {
    string sources;
    auto to_sv = [](char* e) -> string_view {return e;};
    const auto filter_sources = vws::transform(to_sv) | vws::filter(is_source);
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
    string args;
    for (char* v : arg_span) args.append(v) += ',';
    args.pop_back();
    return run(config, &args);
}
auto main(int argc, char** argv) -> int {
    try {
        span<char*> arg_span{argv, static_cast<size_t>(argc)};
        if (arg_span.size() <= 1) {
            println(cerr, red_fmt, "no input files given.");
            return -1;
        }
        string_view first_arg{arg_span[1]};
        if (is_source(first_arg)) return without_command(arg_span);
        println(cerr, "\033[31mUnknown command '{}'.\033[0m", arg_span[1]);
        return -1;
    } catch (std::exception& e) {
        println(cerr, "\033[31mCaught exception '{}'.\033[0m", e.what());
        return -1;
    }
}
