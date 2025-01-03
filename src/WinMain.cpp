#include <iostream>
#include <Windows.h>
#include <lualib.h>
#include <luacode.h>
#include "common.hpp"
#include "win_utility.hpp"
#include "lua_base.hpp"
#include <unordered_map>
static std::unordered_map<std::string, HMODULE> cmodules{};
static int lua_cimport(lua_State* L) {
    const char* dllpath = luaL_checkstring(L, 1);
    const char* symbol = luaL_checkstring(L, 2);
    if (auto it = cmodules.find(dllpath); it == cmodules.end()) {
        HMODULE hModule = LoadLibrary(dllpath);
        if (hModule == NULL) {
            luaL_errorL(L, "failed to load dll");
            return 0;
        }
        cmodules.emplace(std::string(dllpath), hModule);
    }
    HMODULE hmodule = cmodules[dllpath];
    FARPROC pProc = GetProcAddress(hmodule, symbol);
    if (pProc == NULL) {
        luaL_errorL(L, "failed to get proc address");
        FreeLibrary(hmodule);
        return 0;
    }
    lua_pushcfunction(L, (lua_CFunction)pProc, symbol);
    return 1;
}
const luaL_Reg lib[] = {
    {"cimport", lua_cimport},
    {nullptr, nullptr}
};
static int16_t useratom(const char* key, size_t size) {
    static int16_t current_number{1};
    return current_number++;
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
    int exit_code = halua_main(std::string_view(lpCmdLine));
    if (not alloc) win_utility::simulate_key_press(VK_RETURN);
    //std::cout << std::flush;
    //std::cerr << std::flush;
    FreeConsole();
    return exit_code;
}
