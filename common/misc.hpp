#pragma once
#include <fstream>
#include <filesystem>
#include <string>
#include <optional>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace misc {
inline std::optional<std::string> read_file(const std::filesystem::path &path) {
    namespace fs = std::filesystem;
    if (not fs::exists(path)) [[unlikely]] {
        return std::nullopt;
    }
    std::ifstream file_in{};
    file_in.open(path.c_str());
    if  (not file_in.is_open()) [[unlikely]] {
        return std::nullopt;
    }
    std::string curr_line{};
    std::stringstream file_stream{};
    while (std::getline(file_in,curr_line)) {
        file_stream << curr_line << '\n';
    }
    return file_stream.str();
}
inline std::optional<std::filesystem::path> get_executable_path() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    if (length == 0) return std::nullopt;
    return std::string(buffer, length);
#else /*linux*/
    char buffer[PATH_MAX];
    ssize_t length = readlink("/proc/self/exe", buffer, PATH_MAX);
    if (length == -1) return std::nullopt;
    return std::string(buffer, length);
#endif
}
}
