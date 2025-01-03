#pragma once
#include <optional>
#include <filesystem>
#include <string>
#include <variant>
#include "Error.hpp"
struct lua_State;
[[nodiscard]] std::optional<std::string> read_file(const std::filesystem::path& path);
[[nodiscard]] std::variant<lua_State*, Error> load_script(lua_State* L, const std::filesystem::path& script_path) noexcept;
[[nodiscard]] std::optional<Error> spawn_script(lua_State* L, const std::filesystem::path& script_path) noexcept;
template <class ...Ts>
void print(Ts&&...args) {
    ((std::cout << args << ' '), ...) << '\n';
}
template <class ...Ts>
void printerr(Ts&&...args) {
    std::cerr << "\033[31m";//red
    ((std::cerr << args << ' '), ...) << "\033[0m\n";
}
