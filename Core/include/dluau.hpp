#pragma once
#include "dluau.h"
#include <span>
#include <string>
#include <format>
#include <filesystem>
#include <expected>
#ifdef DLUAUCORE_EXPORT
#include <dluau_ex.hpp>
#endif

namespace dluau {
template<class Ty>
constexpr auto to_opaque(lua_State* L, int idx) -> Ty* {
    return static_cast<Ty*>(dluau_toopaque(L, idx));
}
template<class Ty>
constexpr auto check_opaque(lua_State* L, int idx) -> Ty* {
    return static_cast<Ty*>(dluau_checkopaque(L, idx));
}
inline auto check_vector(lua_State* L, int idx) -> std::span<const float> {
    return std::span<const float>{luaL_checkvector(L, idx), LUA_VECTOR_SIZE};
}
inline auto to_vector(lua_State* L, int idx) -> std::span<const float> {
    return std::span<const float>{lua_tovector(L, idx), LUA_VECTOR_SIZE};
}
inline void push(lua_State* L, std::span<const float> vector) {
    lua_pushvector(L, vector[0], vector[1], vector[2]);
} 
template <class ...Tys>
[[noreturn]] constexpr void error(lua_State* L, const std::format_string<Tys...>& fmt, Tys&&...args) {
    luaL_errorL(L, std::format(fmt, std::forward<Tys>(args)...).c_str());
}
[[noreturn]] inline void error(lua_State* L, const std::string& msg) {
    luaL_errorL(L, msg.c_str());
}
template <class ...Ts>
[[noreturn]] constexpr void arg_error(lua_State* L, int idx, const std::format_string<Ts...>& fmt, Ts&&...args) {
    luaL_argerror(L, idx, format(fmt, std::forward<Ts>(args)...).c_str());
}
[[noreturn]] inline void arg_error(lua_State* L, int idx, const std::string& msg) {
    luaL_argerrorL(L, idx, msg.c_str());
}
template <class ...Ts>
[[noreturn]] constexpr void type_error(lua_State* L, int idx, const std::format_string<Ts...>& fmt, Ts&&...args) {
    luaL_typeerrorL(L, idx, format(fmt, std::forward<Ts>(args)...).c_str());
}
[[noreturn]] inline void type_error(lua_State* L, int idx, const std::string& msg) {
    luaL_typeerrorL(L, idx, msg.c_str());
}
[[nodiscard]] inline std::string_view tostring(lua_State* L, int idx) {
    size_t len;
    const char* str = luaL_tolstring(L, idx, &len);
    return {str, len};
}
template <class ...Ts>
void push(lua_State* L, const std::format_string<Ts...>& fmt, Ts&&...args) {
    lua_pushstring(L, std::format(fmt, std::forward<Ts>(args)...).c_str());
}
inline void push(lua_State* L, std::string_view string) {
    lua_pushlstring(L, string.data(), string.length());
}
inline void push(lua_State* L, const char* string) {
    lua_pushstring(L, string);
}
inline void push(lua_State* L, const std::string& string) {
    lua_pushlstring(L, string.data(), string.length());
}
inline void push(lua_State* L, const std::filesystem::path& string) {
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
