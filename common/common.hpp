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
#include <shlobj.h>
#endif

namespace common {
struct raii {
    using fn = std::function<void()>;
    fn sr;
    raii() noexcept = default;
    explicit raii(fn subroutine) noexcept : sr(std::move(subroutine)) {}
    raii(const raii&) = delete;
    raii& operator=(const raii&) = delete;
    raii(raii&& other) noexcept = default;
    raii& operator=(raii&& other) noexcept = default;
    ~raii() { if (sr) sr(); }
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
inline std::optional<std::string> find_environment_variable(const std::string& name) {
    if (const char* env = getenv(name.c_str())) {
        return env;
    }
    return std::nullopt;
}
inline std::optional<std::string> get_user_folder() {
    char user[MAX_PATH];
    const bool success = SUCCEEDED(
        SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, user)
    );
    if (success) {
        std::string folder{user};
        std::ranges::replace(folder, '\\', '/');
        return folder;
    }
    return std::nullopt;
}
inline std::optional<std::string> substitute_user_folder(std::string_view p) {
    if (p[0] != '~') return std::nullopt;
    if (auto opt = get_user_folder()) {
        std::string path{p};
        path.replace(0, 1, *opt);
        return path;
    }
    return std::nullopt;
}
inline std::optional<std::string> substitute_c_drive(std::string_view p) {
    if (p[0] != 'C' or p[1] != ':') return std::nullopt;
        return std::string(p.substr(2));
}
inline std::optional<std::string> substitute_environment_variable(std::string_view p) {
    std::string str{p};
    std::smatch sm;
    static const std::regex env_var_specified{R"(\$[A-Za-z][A-Za-z0-9_-]*)"};
    if (std::regex_search(str, sm, env_var_specified)) {
        const std::string var = sm.str().substr(1);
        if (auto env = find_environment_variable(var)) {
            str.replace(sm.position(), sm.length(), *env); 
            return str;
        } else {
            return std::nullopt;
        }
    }
    return std::nullopt;
}
inline std::string sanitize_path(std::string_view p, const std::filesystem::path& cwd = std::filesystem::current_path()) {
    std::string str{p};
    std::smatch sm;
    std::filesystem::path path{p};
    if (path.is_relative()) path = cwd / path;
    str = std::filesystem::absolute(path).string();
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
