#include <Windows.h>
#include <lualib.h>
#include <luacode.h>
#include "luminutils.h"
#include <Error_info.hpp>
#include <ranges>
#include <filesystem>
#include <lumin.h>
#include <optional>
#include <format>
#include <iostream>
#include <memory>
#include <variant>
#include <misc.hpp>
#include <functional>
#include <vector>
#include "config_utils.hpp"
static void output_error(std::string_view msg) {
    std::cerr << std::format("\033[31m{}\033[0m\n", msg);
}
static int run_process(std::span<char> zarg) {
    STARTUPINFO si = { sizeof(si) };  // Use current console
    PROCESS_INFORMATION pi = { 0 };
    // Create the child process
    if (CreateProcessA(
            NULL,                      // Application name (can be NULL if using command line)
            zarg.data(),            // Command line
            NULL,                      // Process security attributes
            NULL,                      // Thread security attributes
            TRUE,                      // Inherit handles (allows sharing the console)
            0,                         // Creation flags (0 means no new console)
            NULL,                      // Use parent's environment
            NULL,                      // Use parent's current directory
            &si,                       // Startup info
            &pi                        // Process information
        )) {
        // Wait for the child process to finish
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Get the exit code of the child process
        DWORD exitCode;
        if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
            std::cout << "Child process exited with code: " << exitCode << std::endl;
        } else {
            std::cerr << "Failed to get exit code.\n";
        }

        // Clean up handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        std::cerr << "Failed to start process. Error: " << GetLastError() << "\n";
    }
    return 0;
}

static int run(const Project_configuration& config, std::optional<std::span<std::string_view>> args) {
    lumin_Launch_options opts{
        .scripts = config.sources.c_str(),
        .args = nullptr,
    };
    if (args) {
        static std::string argdata;
        for (auto arg : *args) {
            argdata += arg;
            argdata += ' ';
        }
        argdata.pop_back();
        opts.args = argdata.c_str();
    }
    return lumin_run(&opts);
    //std::string cmd = "./-bin/debug/lumin-host.console.exe --sources=test2.luau";
    //return run_process(cmd);
    //return std::system("powershell ./-bin/debug/lumin-host.console.exe --sources=test2.luau");
}
int lumin_main(std::span<std::string_view> args) {
    std::optional<std::span<std::string_view>> run_args;
    auto it = std::ranges::find(args, "--");
    if (it != args.end() and it != args.end() - 1) {
        run_args = args.subspan(++it - args.begin());
    }
    if (args[0] == "run") {
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
        return run(config, run_args);
    }
    return 0;
}
