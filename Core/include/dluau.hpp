#pragma once
#include "dluau.h"
#include <string>
#include <format>
#include <filesystem>
#include <regex>
#include <expected>
#include <optional>
#include <chrono>
#include <span>
#include <ranges>
#include <variant>
#include <memory>
#include <boost/container/flat_map.hpp>
#include <regex>

namespace dluau {
using boost::container::flat_map;
using namespace std::string_literals;
using std::string, std::string_view;
using std::span, std::array;
using std::expected, std::unexpected;
using std::format_string, std::format;
namespace fs = std::filesystem;
extern lua_CompileOptions* compile_options;
static const array def_file_exts = {".luau"s, ".lua"s};
constexpr char arg_separator{','};
inline string_view args;
auto get_script_paths() -> const flat_map<lua_State*, string>&;
auto get_aliases() -> const flat_map<string, string>&;
auto resolve_require_path(lua_State* L, string name, span<const string> file_exts = def_file_exts) -> expected<string, string>;
auto resolve_path(string name, const fs::path& base, span<const string> = def_file_exts) -> expected<string, string>;
auto load_file(lua_State* L, string_view path) -> expected<lua_State*, string>;
auto run_file(lua_State* L, string_view script_path) -> expected<void, string>;
auto tasks_in_progress() -> bool;
auto task_step(lua_State* L) -> expected<void, string>;
auto has_permissions(lua_State* L) -> bool;
auto default_useratom(const char* key, size_t len) -> int16_t;
auto precompile(string& source) -> bool;
auto precompile(string& source, span<const std::pair<std::regex, string>> sv) -> bool;
namespace detail {
auto get_script_paths() -> flat_map<lua_State*, string>&;
}

inline auto get_precompiled_library_values(const fs::path& p) -> decltype(auto) {
    auto as_string_literal = [](const fs::path& path) {
        auto str = path.string();
        std::ranges::replace(str, '\\', '/');
        return format("(\"{}\")", str);
    };
    const auto arr = std::to_array<std::pair<std::regex, string>>({
        {std::regex(R"(\bscript.directory\b)"), as_string_literal(p.parent_path())},
        {std::regex(R"(\bscript.path\b)"), as_string_literal(p)},
        {std::regex(R"(\bscript.name\b)"), as_string_literal(fs::path(p).stem())},
    });
    return arr;
}
template<class Ty>
constexpr auto to_opaque(lua_State* L, int idx) -> Ty* {
    return static_cast<Ty*>(dluau_toopaque(L, idx));
}
template<class Ty>
constexpr auto check_opaque(lua_State* L, int idx) -> Ty* {
    return static_cast<Ty*>(dluau_checkopaque(L, idx));
}
template <class ...Tys>
[[noreturn]] constexpr void error(lua_State* L, const format_string<Tys...>& fmt, Tys&&...args) {
    luaL_errorL(L, format(fmt, std::forward<Tys>(args)...).c_str());
}
[[noreturn]] inline void error(lua_State* L, const std::string& msg) {
    luaL_errorL(L, msg.c_str());
}
template <class ...Ts>
[[noreturn]] constexpr void arg_error(lua_State* L, int idx, const format_string<Ts...>& fmt, Ts&&...args) {
    luaL_argerror(L, idx, format(fmt, std::forward<Ts>(args)...).c_str());
}
[[noreturn]] inline void arg_error(lua_State* L, int idx, const string& msg) {
    luaL_argerrorL(L, idx, msg.c_str());
}
template <class ...Ts>
[[noreturn]] constexpr void type_error(lua_State* L, int idx, const format_string<Ts...>& fmt, Ts&&...args) {
    luaL_typeerrorL(L, idx, format(fmt, std::forward<Ts>(args)...).c_str());
}
[[noreturn]] inline void type_error(lua_State* L, int idx, const string& msg) {
    luaL_typeerrorL(L, idx, msg.c_str());
}
template <class ...Ts>
void push(lua_State* L, const format_string<Ts...>& fmt, Ts&&...args) {
    lua_pushstring(L, std::format(fmt, std::forward<Ts>(args)...).c_str());
}
inline void push(lua_State* L, string_view string) {
    lua_pushlstring(L, string.data(), string.length());
}
inline void push(lua_State* L, const char* string) {
    lua_pushstring(L, string);
}
inline void push(lua_State* L, const string& string) {
    lua_pushlstring(L, string.data(), string.length());
}
inline void push(lua_State* L, const fs::path& string) {
    lua_pushstring(L, string.string().c_str());
}
inline void push(lua_State* L, double number) {
    lua_pushnumber(L, number);
}
inline void push(lua_State* L, int integer) {
    lua_pushinteger(L, integer);
}
inline void push(lua_State* L, bool boolean) {
    lua_pushboolean(L, boolean);
}
template <class Ty = void>
auto to_userdata_tagged(lua_State* L, int idx, int tag) -> Ty& {
    return *static_cast<Ty*>(lua_touserdatatagged(L, idx, tag));
}
template <class Ty = void>
auto to_userdata(lua_State* L, int idx) -> Ty& {
    return *static_cast<Ty*>(lua_touserdata(L, idx));
}
template <class Ty>
auto check_userdata_tagged(lua_State* L, int idx, int tag) -> Ty& {
    if (lua_userdatatag(L, idx) != tag) type_error(L, typeid(Ty).name());
    return *static_cast<Ty*>(lua_touserdatatagged(L, idx, tag));
}
template <class Ty>
auto new_userdata_tagged(lua_State* L, int tag) -> Ty& {
    return *static_cast<Ty*>(lua_newuserdatatagged(L, sizeof(Ty), tag));
}
template <class Ty>
void default_dtor(void* ud) {
    static_cast<Ty*>(ud)->~Ty();
}
template <class Ty>
auto new_userdata(lua_State* L, void(*dtor)(void*) = default_dtor<Ty>) -> Ty& {
    return *static_cast<Ty*>(lua_newuserdatadtor(L, sizeof(Ty), dtor));
}
template <class Ty, std::constructible_from<Ty> ...Params>
auto make_userdata(lua_State* L, Params&&...args) -> Ty& {
    Ty* ud = static_cast<Ty*>(lua_newuserdatadtor(L, sizeof(Ty), default_dtor<Ty>));
    new (ud) Ty{std::forward<Params>(args)...};
    return *ud;
}
}
