#pragma once
#include <fstream>
#include <filesystem>
#include <string>
#include <optional>
#include <common/error_trail.hpp>
#include <functional>
#include <regex>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace common {
struct raii {
    std::function<void()> subroutine;
    ~raii() {subroutine();}
};
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
inline std::optional<std::string> sanitize_path(std::string_view path) {
    std::string str{path};
    std::smatch sm;
    static const std::regex env_var_specified{R"(\$[A-Za-z][A-Za-z0-9_-]*)"};
    if (std::regex_search(str, sm, env_var_specified)) {
        const std::string var = sm.str().substr(1);
        if (const char* env = getenv(var.c_str())) {
            str.replace(sm.position(), sm.length(), env); 
        } else {
            return std::nullopt;
        }
    } else {
        str = std::filesystem::absolute(path).string();
    }
    std::ranges::replace(str, '\\', '/');
    if (str.ends_with('/')) str.pop_back();
    return str;
} 
inline std::string make_path_pretty(std::string_view path) {
    std::string str{path};
    std::smatch sm;
    static const std::regex c_drive_specified(R"([Cc]:)");
    if (std::regex_search(str, sm, c_drive_specified)){
        str.erase(sm.position(), sm.length());
    }
    static const std::regex user_folder_specified(R"([\\\/][Uu]sers[\\\/][A-Za-z0-9 _-]*)");
    if (std::regex_search(str, sm, user_folder_specified)) str.replace(sm.position(), sm.length(), "~");
    return str;
}
}
