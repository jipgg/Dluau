#pragma once
#include "dluau.h"
#include <string>
#include <format>
#include <filesystem>

namespace dluau {
template <class ...Ts>
[[noreturn]] constexpr void error(lua_State* L, const std::format_string<Ts...>& fmt, Ts&&...args) {
    luaL_errorL(L, std::format(fmt, std::forward<Ts>(args)...).c_str());
}
[[noreturn]] inline void error(lua_State* L, const std::string& msg) {
    luaL_errorL(L, msg.c_str());
}
template <class ...Ts>
[[noreturn]] constexpr void argerror(lua_State* L, int idx, const std::format_string<Ts...>& fmt, Ts&&...args) {
    luaL_argerror(L, idx, std::format(fmt, std::forward<Ts>(args)...).c_str());
}
[[noreturn]] inline void argerror(lua_State* L, int idx, const std::string& msg) {
    luaL_argerrorL(L, idx, msg.c_str());
}
template <class ...Ts>
[[noreturn]] constexpr void typeerror(lua_State* L, int idx, const std::format_string<Ts...>& fmt, Ts&&...args) {
    luaL_typeerrorL(L, idx, std::format(fmt, std::forward<Ts>(args)...).c_str());
}
[[noreturn]] inline void typeerror(lua_State* L, int idx, const std::string& msg) {
    luaL_typeerrorL(L, idx, msg.c_str());
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
template <class T = void>
T& toudtagged(lua_State* L, int idx, int tag) {
    return *static_cast<T*>(lua_touserdatatagged(L, idx, tag));
}
template <class T = void>
T& touserdata(lua_State* L, int idx) {
    return *static_cast<T*>(lua_touserdata(L, idx));
}
template <class T>
T& checkudtagged(lua_State* L, int idx, int tag) {
    if (lua_userdatatag(L, idx) != tag) typeerror(L, typeid(T).name());
    return *static_cast<T*>(lua_touserdatatagged(L, idx, tag));
}
template <class T>
T& newudtagged(lua_State* L, int tag) {
    return *static_cast<T*>(lua_newuserdatatagged(L, sizeof(T), tag));
}
template <class T>
void default_dtor(void* ud) {
    static_cast<T*>(ud)->~T();
}
template <class T>
T& newuserdata(lua_State* L, void(*dtor)(void*) = default_dtor<T>) {
    return *static_cast<T*>(lua_newuserdatadtor(L, sizeof(T), dtor));
}
template <class T, std::constructible_from<T> ...Params>
T& make_userdata(lua_State* L, Params&&...args) {
    T* ud = static_cast<T*>(lua_newuserdatadtor(L, sizeof(T), default_dtor<T>));
    new (ud) T{std::forward<Params>(args)...};
    return *ud;
}
}
