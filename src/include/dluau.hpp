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

namespace dluau::type_aliases {
namespace fs = std::filesystem;
namespace views = std::views;
namespace ranges = std::ranges;
namespace chrono = std::chrono;
template <class Key, class T> using Flat_map = boost::container::flat_map<Key, T>;
using Millisec = chrono::milliseconds;
template <class Rep, class Period = std::ratio<1>> using Duration = chrono::duration<Rep, Period>;
template <class ...Ts> using Tuple = std::tuple<Ts...>;
using Lstate = lua_State*;
using Lthread = lua_State*;
using Lreg = luaL_Reg;
using String = std::string;
using Path = std::filesystem::path;
using Regex = std::regex;
using Smatch = std::smatch;
using Sregex_iterator = std::sregex_iterator;
using Strview = std::string_view;
template <class Fty> using Func = std::function<Fty>;
template <class T> using Vector = std::vector<T>;
template <class T, size_t N> using Array = std::array<T, N>;
template <class T, class U> using Pair = std::pair<T, U>;
template <class T> using Span = std::span<T>;
template <class T, class Err = String> using Expected = std::expected<T, Err>;
template <class Err = String> using Unexpected = std::unexpected<Err>;
template <class ...Ts> using Variant = std::variant<Ts...>;
template <class T> using Opt = std::optional<T>;
template <class T> using Ref = std::reference_wrapper<T>;
template <class T, class Dx = std::default_delete<T>> using Unique = std::unique_ptr<T, Dx>;
template <class T> using Shared = std::shared_ptr<T>;
}

namespace dluau {
using namespace type_aliases;
const Flat_map<lua_State*, String>& get_script_paths();
const Flat_map<String, String>& get_aliases();
extern lua_CompileOptions* compile_options;
static const auto default_file_extensions = std::to_array<String>({".luau", ".lua"});
Expected<String> resolve_require_path(lua_State* L, String name, Span<const String> file_exts = default_file_extensions);
Expected<String> resolve_path(String name, const Path& base, Span<const String> file_exts = default_file_extensions);
Expected<lua_State*> load_file(lua_State* L, Strview path);
Expected<void> run_file(lua_State* L, Strview script_path);
bool tasks_in_progress();
Expected<void> task_step(lua_State* L);
bool has_permissions(lua_State* L);
constexpr char arg_separator{','};
int16_t default_useratom(const char* key, size_t len);
inline Strview args;
bool precompile(String& source);
bool precompile(String& source, Span<const std::pair<Regex, String>> sv);

template <class ...Ts>
[[noreturn]] constexpr void error(lua_State* L, const std::format_string<Ts...>& fmt, Ts&&...args) {
    luaL_errorL(L, std::format(fmt, std::forward<Ts>(args)...).c_str());
}
[[noreturn]] inline void error(lua_State* L, const std::string& msg) {
    luaL_errorL(L, msg.c_str());
}
template <class ...Ts>
[[noreturn]] constexpr void arg_error(lua_State* L, int idx, const std::format_string<Ts...>& fmt, Ts&&...args) {
    luaL_argerror(L, idx, std::format(fmt, std::forward<Ts>(args)...).c_str());
}
[[noreturn]] inline void arg_error(lua_State* L, int idx, const String& msg) {
    luaL_argerrorL(L, idx, msg.c_str());
}
template <class ...Ts>
[[noreturn]] constexpr void type_error(lua_State* L, int idx, const std::format_string<Ts...>& fmt, Ts&&...args) {
    luaL_typeerrorL(L, idx, std::format(fmt, std::forward<Ts>(args)...).c_str());
}
[[noreturn]] inline void type_error(lua_State* L, int idx, const String& msg) {
    luaL_typeerrorL(L, idx, msg.c_str());
}
template <class ...Ts>
void push(lua_State* L, const std::format_string<Ts...>& fmt, Ts&&...args) {
    lua_pushstring(L, std::format(fmt, std::forward<Ts>(args)...).c_str());
}
inline void push(lua_State* L, Strview string) {
    lua_pushlstring(L, string.data(), string.length());
}
inline void push(lua_State* L, const char* string) {
    lua_pushstring(L, string);
}
inline void push(lua_State* L, const String& string) {
    lua_pushlstring(L, string.data(), string.length());
}
inline void push(lua_State* L, const Path& string) {
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
template <class T = void>
T& to_userdata_tagged(lua_State* L, int idx, int tag) {
    return *static_cast<T*>(lua_touserdatatagged(L, idx, tag));
}
template <class T = void>
T& to_userdata(lua_State* L, int idx) {
    return *static_cast<T*>(lua_touserdata(L, idx));
}
template <class T>
T& check_userdata_tagged(lua_State* L, int idx, int tag) {
    if (lua_userdatatag(L, idx) != tag) type_error(L, typeid(T).name());
    return *static_cast<T*>(lua_touserdatatagged(L, idx, tag));
}
template <class T>
T& new_userdata_tagged(lua_State* L, int tag) {
    return *static_cast<T*>(lua_newuserdatatagged(L, sizeof(T), tag));
}
template <class T>
void default_dtor(void* ud) {
    static_cast<T*>(ud)->~T();
}
template <class T>
T& new_userdata(lua_State* L, void(*dtor)(void*) = default_dtor<T>) {
    return *static_cast<T*>(lua_newuserdatadtor(L, sizeof(T), dtor));
}
template <class T, std::constructible_from<T> ...Params>
T& make_userdata(lua_State* L, Params&&...args) {
    T* ud = static_cast<T*>(lua_newuserdatadtor(L, sizeof(T), default_dtor<T>));
    new (ud) T{std::forward<Params>(args)...};
    return *ud;
}
}
