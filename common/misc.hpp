#pragma once
#include <fstream>
#include <filesystem>
#include <string>
#include <optional>

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
}
