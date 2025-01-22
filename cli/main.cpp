#include <Windows.h>
#include <lualib.h>
#include <luacode.h>
#include "luminutils.h"
#include <Error_info.hpp>
#include <ranges>
#include <filesystem>
#include <goluau.h>
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
static void output_error(std::string_view msg) {
    std::cerr << std::format("\033[31m{}\033[0m\n", msg);
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
        std::cerr << "Failed to start process. Error: " << GetLastError() << "\n";
        return -1;
    }
    WaitForSingleObject(process_info.hProcess, INFINITE);
    DWORD exit_code = -1;
    GetExitCodeProcess(process_info.hProcess, &exit_code);
    return exit_code;
}
static int do_run_command(const Project_configuration& config, const std::string* const args) {
    using namespace std::string_literals;
    std::string exe {std::format("./-bin/debug/LuauXT_host.exe")};
    std::string cmd = std::format("{} --sources={}", exe, config.sources);
    if (args) cmd += std::format(" --args={}", *args);
    cmd += std::format(" -O{}", config.optimization_level);
    auto exit_code = run(cmd);
    return exit_code;
}
int main(int argc, char** argv) {
    try {
        std::span<char*> arg_span{argv, static_cast<size_t>(argc)};
        if (arg_span[1] == std::string_view("run")) {
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
            const auto& config = std::get<Project_configuration>(config_var);
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
                return do_run_command(config, &args);
            }
            return do_run_command(config, nullptr);
        }
        std::cerr << std::format("\033[31mUnknown command '{}'.\033[0m\n", arg_span[1]);
        return -1;
    } catch (std::exception& e) {
        std::cerr << std::format("\033[31mException thrown '{}'.\033[0m\n", e.what());
        return -1;
    }
}
