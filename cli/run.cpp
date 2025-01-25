#include "cli.hpp"
#include <iostream>
#include <span>
#include <format>
#include <common.hpp>
#include <functional>
using std::string, std::format, std::span;
using cli::configuration;
#ifdef _WIN32
#include <Windows.h>
constexpr std::string_view runtime_host_name{"dluau-windows-host.exe"};

static int run_windows_host(span<char> zarg) {
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
    struct raii {
        std::function<void()> subroutine;
        ~raii() {subroutine();}
    } close_handles([&process_info] {
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
int cli::run(const configuration& config, const string* const args) {
    using namespace std::string_literals;
    auto path = common::get_executable_path();
    if (not path) {
        std::cerr << "\033[31mFailed to get executable path.\033[0m\n";
        return -2;
    }
    string exe = (path->parent_path() / runtime_host_name).string();
    string cmd = format("{} --sources={}", exe, config.sources);
    if (args) cmd += format(" --args={}", *args);
    cmd += format(" -O{}", config.optimization_level);
    auto exit_code = run_windows_host(cmd);
    return exit_code;
}
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
