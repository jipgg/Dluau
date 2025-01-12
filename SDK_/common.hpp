#pragma once
#include <optional>
#include <filesystem>
#include <string>
#include <variant>
#include "error_info.hpp"
struct lua_State;
[[nodiscard]] std::optional<std::string> read_file(const std::filesystem::path& path);
[[nodiscard]] std::variant<lua_State*, error_info> load_script(lua_State* L, const std::filesystem::path& script_path) noexcept;
[[nodiscard]] std::optional<error_info> spawn_script(lua_State* L, const std::filesystem::path& script_path) noexcept;
template <class T>
concept with_overloaded_ostream = requires(std::ostream& os, T v) {
    {os << v} -> std::same_as<std::ostream&>;
}; 
template <with_overloaded_ostream...Ts>
void print(Ts&&...args) {
    ((std::cout << args << ' '), ...) << '\n';
}
template <with_overloaded_ostream...Ts>
void printerr(Ts&&...args) {
    std::cerr << "\033[31m";//red
    ((std::cerr << args << ' '), ...) << "\033[0m\n";
}
