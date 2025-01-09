#include <iostream>
#include <Windows.h>
#include <lualib.h>
#include <luacode.h>
#include "common.hpp"
#include "win_utility.hpp"
#include "lua_base.hpp"
#include <minlu.h>
#include <unordered_map>
static std::unordered_map<std::string, HMODULE> cmodules{};
const luaL_Reg lib[] = {
    {"dllimport", minlufn_dllimport},
    {nullptr, nullptr}
};
static int16_t useratom(const char* key, size_t size) {
    static int16_t current_number{1};
    return current_number++;
}
void ConfigureConsoleInputMode() {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to get standard input handle. Error: " << GetLastError() << std::endl;
        return;
    }

    // Set console input mode
    DWORD mode;
    if (!GetConsoleMode(hStdin, &mode)) {
        std::cerr << "Failed to get console mode. Error: " << GetLastError() << std::endl;
        return;
    }

    // Enable input processing (e.g., line input, echo input, etc.)
    mode |= ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT;
    if (!SetConsoleMode(hStdin, mode)) {
        std::cerr << "Failed to set console mode. Error: " << GetLastError() << std::endl;
    }

    // Flush the input buffer
    FlushConsoleInputBuffer(hStdin);
}
int halua_main(std::string_view args) {
    lua_State* L = luaL_newstate();
    lua_callbacks(L)->useratom = useratom;
    luaL_openlibs(L);
    lua_register_globals(L);
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, nullptr, lib);
    lua_pop(L, 1);
    if (auto error = spawn_script(L, "test.luau")) {
        printerr(error->message());
        return 1;
    }
    lua_close(L);
    for (auto& [path, module] : cmodules) {
        FreeLibrary(module);
    }
    return 0;
}
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    const bool alloc = win_utility::redirect_console_output();
    win_utility::enable_virtual_terminal_processing();
    ConfigureConsoleInputMode();
    int exit_code = halua_main(std::string_view(lpCmdLine));
    print("adadadaadadadd END");
    if (not alloc) win_utility::simulate_key_press(VK_RETURN);
    std::cout << std::flush;
    std::cerr << std::flush;
    FreeConsole();
    return exit_code;
}
