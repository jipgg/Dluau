#pragma once
#include <fstream>
#include <filesystem>
#include <string>
#include <optional>
#include <functional>
#include <expected>
#ifdef _WIN32
#include <Windows.h>
#include <shlobj.h>
#endif

namespace common {
struct Raii {
    using Fn = std::function<void()>;
    Fn sr;
    Raii() noexcept = default;
    explicit Raii(Fn subroutine) noexcept : sr(std::move(subroutine)) {}
    Raii(const Raii&) = delete;
    Raii& operator=(const Raii&) = delete;
    Raii(Raii&& other) noexcept = default;
    Raii& operator=(Raii&& other) noexcept = default;
    ~Raii() { if (sr) sr(); }
};
inline auto read_file(const std::filesystem::path &path) -> std::optional<std::string> {
    if (not std::filesystem::exists(path)) [[unlikely]] {
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
inline auto get_bin_path() -> std::optional<std::filesystem::path> {
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
inline auto find_environment_variable(const std::string& name) -> std::optional<std::string> {
    if (const char* env = getenv(name.c_str())) {
        return env;
    }
    return std::nullopt;
}
inline auto get_user_folder() -> std::optional<std::string> {
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
inline auto substitute_user_folder(const std::filesystem::path& p) -> std::expected<std::filesystem::path, std::string> {
    if (p.string()[0] != '~') return p;
    if (auto opt = get_user_folder()) {
        auto path{p.string()};
        path.replace(0, 1, *opt);
        return std::filesystem::path(path);
    }
    return std::unexpected("could not get user folder");
}
inline auto normalize_path(
    std::filesystem::path path,
    const std::filesystem::path& base = std::filesystem::current_path()) -> decltype(path) {
    if (path.is_relative()) {
        path = base / path;
    }
    path = std::filesystem::weakly_canonical(path);
    std::string str = path.string();
    std::ranges::replace(str, '\\', '/');
    return str;
}
}
