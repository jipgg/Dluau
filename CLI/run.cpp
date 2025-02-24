#include "cli.hpp"
#include <iostream>
#include <span>
#include <format>
#include <print>
#include <common.hpp>
#include <functional>
using cli::Configuration;
#ifdef _WIN32
#include <Windows.h>
using std::string_view, std::string;
using std::format, std::span;
using common::Raii;
constexpr string_view runtime_host_name{"dluau-windows-host.exe"};

static auto run_windows_host(span<char> zarg) -> int {
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
    Raii close_handles([&pi = process_info] {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
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

auto cli::run(const Configuration& config, const string* const args) -> int {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
    const dluau_InitOptions opts {
        .scripts = config.sources.c_str(),
        .args = args->c_str(),
        .debug_level = config.debug_level,
        .optimization_level = config.optimization_level,
    };
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    const int exit_code = dluau_run(&opts);
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    return exit_code;
}
/*auto cli::run(const Configuration& config, const string* const args) -> int {*/
/*    using namespace std::string_literals;*/
/*    auto path = common::get_bin_path();*/
/*    if (not path) {*/
/*        std::println(std::cerr, "\033[31mFailed to get executable path.\033[0m");*/
/*        return -2;*/
/*    }*/
/*    string exe = (path->parent_path() / runtime_host_name).string();*/
/*    string cmd = format("{} --sources={}", exe, config.sources);*/
/*    if (args) cmd += format(" --args={}", *args);*/
/*    cmd += format(" -O{}", config.optimization_level);*/
/*    auto exit_code = run_windows_host(cmd);*/
/*    return exit_code;*/
/*}*/
#else
int cli::run(const configuration& config, const string* const args) {
    const dluau_RunOptions opts {
        .scripts = config.sources.c_str(),
        .args = args->c_str(),
        .debug_level = config.debug_level,
        .optimization_level = config.optimization_level,
    };
    const int exit_code = dluau_run(&opts);
    return exit_code;
}
#endif
