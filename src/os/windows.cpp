#include "os.hpp"
#include <common.hpp>
#ifdef _WIN32
#include "Windows.h"

static auto message_box(lua_State* L) -> int {
    const int status = MessageBox(
        nullptr,
        luaL_checkstring(L, 1),
        luaL_optstring(L, 2, "Message Box"),
        luaL_optinteger(L, 3, MB_OK)
    );
    lua_pushinteger(L, status);
    return 1;
}
static auto get_last_error(lua_State* L) -> int {
    lua_pushinteger(L, GetLastError());
    return 1;
}
static auto format_message(lua_State* L) -> int {
    const int ec = luaL_checkinteger(L, 1);
    LPSTR message_buffer = nullptr;
    size_t size = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, ec, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&message_buffer, 0, nullptr
    );
    lua_pushlstring(L, message_buffer, size);
    LocalFree(message_buffer);
    return 1;
}
static auto load_library(lua_State* L) -> int {
    auto hm = LoadLibrary(luaL_checkstring(L, 1));
    if (not hm) return 0;
    dluau_pushopaque(L, hm);
    return 1;
}
static auto free_library(lua_State* L) -> int {
    const auto b = FreeLibrary(dluau::check_opaque<HINSTANCE__>(L, 1));
    lua_pushboolean(L, b);
    return 1;
}
static auto get_module_handle(lua_State* L) -> int {
    auto push_result = [&L](const char* name) {
        if (auto hinstance = GetModuleHandle(name)) {
            dluau_pushopaque(L, hinstance);
        } else lua_pushnil(L);
    };
    if (lua_isnoneornil(L, 1)) push_result(nullptr);
    else push_result(luaL_checkstring(L, 1));
    return 1;
}
static void set_integer(lua_State* L, const char* field, int val) {
    lua_pushinteger(L, val);
    lua_setfield(L, -2, field);
}
void register_windows_lib(lua_State* L) {
    const luaL_Reg os_windows[] = {
        {"message_box", message_box},
        {"get_last_error", get_last_error},
        {"format_message", format_message},
        {"get_module_handle", get_module_handle},
        {"load_library", load_library},
        {"free_library", free_library},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, os_windows);
    lua_setreadonly(L, -1, true);
    lua_setfield(L, -2, "windows");
}
#else
void register_windows_lib(lua_State* L) {}
#endif

