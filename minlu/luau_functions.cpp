#include "minlu.h"
#include <lualib.h>
#include <unordered_map>
#include <string>
#ifdef _WIN32
#include <Windows.h>
static std::unordered_map<std::string, HMODULE> dllmodules{};
#endif

int minlufn_dllimport(lua_State* L) {
#ifdef _WIN32
    const char* dllpath = luaL_checkstring(L, 1);
    const char* symbol = luaL_checkstring(L, 2);
    if (auto it = dllmodules.find(dllpath); it == dllmodules.end()) {
        HMODULE hmodule = LoadLibrary(dllpath);
        if (hmodule == nullptr) {
            luaL_errorL(L, "Could not find DLL with name '%s'.", dllpath);
            return 0;
        }
        dllmodules.emplace(std::string(dllpath), hmodule);
    }
    HMODULE hmodule = dllmodules[dllpath];
    FARPROC proc = GetProcAddress(hmodule, symbol);
    if (proc == NULL) {
        luaL_errorL(L, "Could not find exported function symbol '%s' in DLL '%s'.", symbol, dllpath);
        FreeLibrary(hmodule);
        return 0;
    }
    lua_pushcfunction(L, (lua_CFunction)proc, symbol);
    return 1;
#else
    luaL_errorL(L, "dllimport is currently not supported for this platform");
#endif
}
