#pragma once
#include <fstream>
#include <filesystem>
#include <string>
#include <optional>
#include <common/error_trail.hpp>
#include <functional>
#include <variant>
#ifdef _WIN32
#include <Windows.h>
#include <shlobj.h>
#endif

namespace common {
namespace fs = std::filesystem;
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
inline std::optional<fs::path> get_bin_path() {
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
inline std::variant<fs::path, error_trail> substitute_user_folder(const fs::path& p) {
    if (p.string()[0] != '~') return p;
    if (auto opt = get_user_folder()) {
        std::string path{p.string()};
        path.replace(0, 1, *opt);
        return fs::path(path);
    }
    return error_trail("could not get user folder");
}
inline fs::path normalize_path(fs::path path, const fs::path& relative = "") {
    if (path.is_relative()) {
        const fs::path& base = relative.empty() ? fs::current_path() : relative;
        path = base / path;
    }
    path = fs::weakly_canonical(path);
    std::string str = path.string();
    std::ranges::replace(str, '\\', '/');
    return str;
}
}
