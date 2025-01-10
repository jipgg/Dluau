#include <iostream>
#include <Windows.h>
#include <lualib.h>
#include <luacode.h>
#include "common.hpp"
#include "winutil.hpp"
#include <lumin.h>
#include <filesystem>
int lumin_main(std::string_view args) {
    lua_State* L = luaL_newstate();
    lua_callbacks(L)->useratom = lumin_useratom;
    luaL_openlibs(L);
    lumin_loadfuncs(L);
    if (auto err = spawn_script(L, "test.luau")) {
        printerr(err->message());
        return -1;
    }
    return 0;
}
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    const bool alloc = winutil::redirect_console_output();
    winutil::enable_virtual_terminal_processing();
    winutil::configure_console_input();
    const int exit_code = lumin_main(std::string_view(lpCmdLine));
    if (not alloc) winutil::simulate_key_press(VK_RETURN);
    std::cout << std::flush;
    std::cerr << std::flush;
    FreeConsole();
    return exit_code;
}
