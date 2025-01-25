#include <Windows.h>
#include <lualib.h>
#include <luacode.h>
#include <ErrorInfo.hpp>
#include <ranges>
#include <filesystem>
#include <dluau.h>
#include <optional>
#include <format>
#include <iostream>
#include <memory>
#include <variant>
#include <misc.hpp>
#include <functional>
#include <vector>
#include "config_utils.hpp"
namespace rn = std::ranges;
namespace rv = std::views;
static void output_error(std::string_view msg) {
    std::cerr << std::format("\033[31m{}\033[0m\n", msg);
}
static int get_optimization_level(std::span<char*> args) {
    constexpr std::string_view upper = "-O";
    auto found = rn::find_if(args, [&upper](char* e) {
        constexpr std::string_view lower = "-o";
        std::string_view sv{e};
        sv = sv.substr(0, upper.size());
        return sv == upper or sv == lower;
    });
    if (found == rn::end(args)) return 0;
    const std::string str = std::string(*found).substr(upper.size());
    return std::stoi(str);
}
static int get_debug_level(std::span<char*> args) {
    constexpr std::string_view upper = "-D";
    auto found = rn::find_if(args, [&upper](char* e) {
        constexpr std::string_view lower = "-d";
        std::string_view sv{e};
        sv = sv.substr(0, upper.size());
        return sv == upper or sv == lower;
    });
    if (found == rn::end(args)) return 1;
    const std::string str = std::string(*found).substr(upper.size());
    return std::stoi(str);
}

struct Deferred {
    std::function<void()> subroutine;
    ~Deferred() {subroutine();}
};

static int run(std::span<char> zarg) {
    STARTUPINFO startup_info = { sizeof(startup_info) }; // use current console
    PROCESS_INFORMATION process_info = { 0 };
    LPCSTR executable_name = nullptr;
    LPSTR command = zarg.data();
    LPSECURITY_ATTRIBUTES process_security = nullptr;
    LPSECURITY_ATTRIBUTES thread_security = nullptr;
    BOOL inherit_handles = true;
    DWORD creation_flags = 0;
    LPVOID environment = nullptr; // == parent process
    LPCSTR current_directory = nullptr; // == parent's current directory

    const bool success = CreateProcessA(
        executable_name, command, process_security, thread_security,
        inherit_handles,creation_flags, environment, current_directory,
        &startup_info, &process_info
    );
    Deferred close_handles([&process_info] {
        CloseHandle(process_info.hProcess);
        CloseHandle(process_info.hThread);
    });
    if (not success) {
        std::cerr << "\033[31mFailed to start process. Error: " << GetLastError() << "\033[0m\n";
        return -1;
    }
    WaitForSingleObject(process_info.hProcess, INFINITE);
    DWORD exit_code = -1;
    GetExitCodeProcess(process_info.hProcess, &exit_code);
    return exit_code;
}
static int run_config(const Config& config, const std::string* const args) {
    using namespace std::string_literals;
    auto path = misc::get_executable_path();
    if (not path) {
        std::cerr << "\033[31mFailed to get executable path.\033[0m\n";
        return -2;
    }
    constexpr std::string_view runtime_host_name{"dluau-runtime-host.exe"};
    std::string exe = (path->parent_path() / runtime_host_name).string();
    std::string cmd = std::format("{} --sources={}", exe, config.sources);
    if (args) cmd += std::format(" --args={}", *args);
    cmd += std::format(" -O{}", config.optimization_level);
    auto exit_code = run(cmd);
    return exit_code;
}
static int without_command(std::span<char*> arg_span) {
    std::string sources;
    const auto filter_sources = rv::filter([](char* e) -> bool {return std::string_view(e).ends_with(".luau");});
    for (auto v : arg_span | filter_sources) {
        sources.append(v) += ",";
    }
    sources.pop_back();
    Config config{
        .sources = std::move(sources),
        .optimization_level = get_optimization_level(arg_span),
        .debug_level = get_debug_level(arg_span),
    };
    std::string args;
    for (char* v : arg_span) args.append(v) += ',';
    args.pop_back();
    return run_config(config, &args);
}
static int with_run_command(std::span<char*> arg_span) {
    auto found_config_path = find_config();
    if (not found_config_path) {
        output_error("couldn't find configuration file");
        return -1;
    }
    auto config_var = read_config(*found_config_path);
    if (auto* err = std::get_if<ErrorInfo>(&config_var)) {
        output_error(err->message());
        return -1;
    }
    const auto& config = std::get<Config>(config_var);
    auto found_arg_pos = rn::find_if(arg_span, [](char* e) -> bool {
        using namespace std::string_view_literals;
        return e == "--"sv;
    });
    if (found_arg_pos < rn::end(arg_span) - 1) {
        std::string args;
        while (++found_arg_pos != rn::end(arg_span)) {
            args += *found_arg_pos;
            args += ',';
        }
        args.pop_back();
        return run_config(config, &args);
    }
    return run_config(config, nullptr);
}
int main(int argc, char** argv) {
    try {
        std::span<char*> arg_span{argv, static_cast<size_t>(argc)};
        if (arg_span.size() <= 1) {
            output_error("no input files given.");
            return -1;
        }
        std::string_view first_arg{arg_span[1]};
        if (first_arg.ends_with(".luau")) return without_command(arg_span);
        else if (first_arg == "run") return with_run_command(arg_span);
        std::cerr << std::format("\033[31mUnknown command '{}'.\033[0m\n", arg_span[1]);
        return -1;
    } catch (std::exception& e) {
        std::cerr << std::format("\033[31mCaught exception '{}'.\033[0m\n", e.what());
        return -1;
    }
}
