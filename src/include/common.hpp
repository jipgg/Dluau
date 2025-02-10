#pragma once
#include <fstream>
#include <filesystem>
#include <string>
#include <optional>
#include <functional>
#include <variant>
#include "dluau.hpp"
#ifdef _WIN32
#include <Windows.h>
#include <shlobj.h>
#endif

namespace common {
using namespace dluau::type_aliases;
struct Raii {
    using Fn = Func<void()>;
    Fn sr;
    Raii() noexcept = default;
    explicit Raii(Fn subroutine) noexcept : sr(std::move(subroutine)) {}
    Raii(const Raii&) = delete;
    Raii& operator=(const Raii&) = delete;
    Raii(Raii&& other) noexcept = default;
    Raii& operator=(Raii&& other) noexcept = default;
    ~Raii() { if (sr) sr(); }
};
inline Opt<String> read_file(const Path &path) {
    if (not fs::exists(path)) [[unlikely]] {
        return std::nullopt;
    }
    std::ifstream file_in{};
    file_in.open(path.c_str());
    if  (not file_in.is_open()) [[unlikely]] {
        return std::nullopt;
    }
    String curr_line{};
    std::stringstream file_stream{};
    while (std::getline(file_in,curr_line)) {
        file_stream << curr_line << '\n';
    }
    return file_stream.str();
}
inline Opt<Path> get_bin_path() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    if (length == 0) return std::nullopt;
    return String(buffer, length);
#else /*linux*/
    char buffer[PATH_MAX];
    ssize_t length = readlink("/proc/self/exe", buffer, PATH_MAX);
    if (length == -1) return std::nullopt;
    return std::string(buffer, length);
#endif
}
inline Opt<String> find_environment_variable(const String& name) {
    if (const char* env = getenv(name.c_str())) {
        return env;
    }
    return std::nullopt;
}
inline Opt<String> get_user_folder() {
    char user[MAX_PATH];
    const bool success = SUCCEEDED(
        SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, user)
    );
    if (success) {
        String folder{user};
        ranges::replace(folder, '\\', '/');
        return folder;
    }
    return std::nullopt;
}
inline Expected<Path> substitute_user_folder(const fs::path& p) {
    if (p.string()[0] != '~') return p;
    if (auto opt = get_user_folder()) {
        String path{p.string()};
        path.replace(0, 1, *opt);
        return Path(path);
    }
    return Unexpected("could not get user folder");
}
inline Path normalize_path(Path path, const Path& base = fs::current_path()) {
    if (path.is_relative()) {
        path = base / path;
    }
    path = fs::weakly_canonical(path);
    String str = path.string();
    ranges::replace(str, '\\', '/');
    return str;
}
}
