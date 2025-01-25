#include <Windows.h>
#include <lualib.h>
#include <luacode.h>
#include <ranges>
#include <dluau.h>
#include <optional>
#include <format>
#include <iostream>
#include <variant>
#include <common.hpp>
#include "config_utils.hpp"
namespace rn = std::ranges;
namespace rv = std::views;
using std::cerr, std::format;
using std::string, std::string_view, std::span;
using cli::configuration;

static void output_error(std::string_view msg) {
    cerr << format("\033[31m{}\033[0m\n", msg);
}
static int get_optimization_level(span<char*> args) {
    constexpr string_view upper = "-O";
    auto found = rn::find_if(args, [&upper](char* e) {
        constexpr string_view lower = "-o";
        string_view sv{e};
        sv = sv.substr(0, upper.size());
        return sv == upper or sv == lower;
    });
    if (found == rn::end(args)) return 0;
    const string str = string(*found).substr(upper.size());
    return std::stoi(str);
}
static int get_debug_level(span<char*> args) {
    constexpr string_view upper = "-D";
    auto found = rn::find_if(args, [&upper](char* e) {
        constexpr string_view lower = "-d";
        string_view sv{e};
        sv = sv.substr(0, upper.size());
        return sv == upper or sv == lower;
    });
    if (found == rn::end(args)) return 1;
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
static bool is_file_or_directory(string_view arg) {
    return arg.ends_with('/') or arg.ends_with(".luau");
}

static int without_command(span<char*> arg_span) {
    string sources;
    const auto filter_sources = rv::transform([](char* e) -> string_view {return e;}) | rv::filter(is_file_or_directory);
    for (auto arg : arg_span | filter_sources) {
        if (arg.ends_with('/')) expand_source_directory(arg, sources);
        else expand_source_file(arg, sources);
    }
    sources.pop_back();
    configuration config{
        .sources = std::move(sources),
        .optimization_level = get_optimization_level(arg_span),
        .debug_level = get_debug_level(arg_span),
    };
    string args;
    for (char* v : arg_span) args.append(v) += ',';
    args.pop_back();
    return run(config, &args);
}
static int with_run_command(span<char*> arg_span) {
    auto found_config_path = find_config();
    if (not found_config_path) {
        output_error("couldn't find configuration file");
        return -1;
    }
    auto config_var = read_config(*found_config_path);
    if (auto* err = std::get_if<error_trail>(&config_var)) {
        output_error(err->message());
        return -1;
    }
    const auto& config = std::get<configuration>(config_var);
    auto found_arg_pos = rn::find_if(arg_span, [](char* e) -> bool {
        using namespace std::string_view_literals;
        return e == "--"sv;
    });
    if (found_arg_pos < rn::end(arg_span) - 1) {
        string args;
        while (++found_arg_pos != rn::end(arg_span)) {
            args += *found_arg_pos;
            args += ',';
        }
        args.pop_back();
        return run(config, &args);
    }
    return run(config, nullptr);
}
int main(int argc, char** argv) {
    try {
        span<char*> arg_span{argv, static_cast<size_t>(argc)};
        if (arg_span.size() <= 1) {
            output_error("no input files given.");
            return -1;
        }
        string_view first_arg{arg_span[1]};
        if (is_file_or_directory(first_arg)) return without_command(arg_span);
        else if (first_arg == "run") return with_run_command(arg_span);
        cerr << format("\033[31mUnknown command '{}'.\033[0m\n", arg_span[1]);
        return -1;
    } catch (std::exception& e) {
        cerr << format("\033[31mCaught exception '{}'.\033[0m\n", e.what());
        return -1;
    }
}
