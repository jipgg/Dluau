#include "os.hpp"
#ifdef _WIN32
#include "Windows.h"
static int message_box(lua_State* L) {
    HINSTANCE hinstance = GetModuleHandle(nullptr);
    if (not hinstance) dluau::error(L, "Failed to get HINSTANCE handle");
    MessageBox(nullptr, luaL_checkstring(L, 1), luaL_optstring(L, 2, "Message Box"), MB_OKCANCEL);
    return 0;
}
void register_windows_lib(lua_State* L) {
    const luaL_Reg os_windows[] = {
        {"message_box", message_box},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, os_windows);
    lua_setfield(L, -2, "windows");
}
#else
void register_windows_lib(lua_State* L) {}
#endif

