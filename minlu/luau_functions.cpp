#include "minlu.h"
#include <lualib.h>
#include <unordered_map>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;
#ifdef _WIN32
#include <Windows.h>
static std::unordered_map<fs::path, HMODULE> dllmodules{};
#endif

int minlufn_dllimport(lua_State* L) {
#ifdef _WIN32
    fs::path dllpath = luaL_checkstring(L, 1);
    if (not dllpath.has_extension()) dllpath += ".dll";
    const char* symbol = luaL_checkstring(L, 2);
    if (auto it = dllmodules.find(dllpath); it == dllmodules.end()) {
        const std::string dllpath_str = dllpath.string();
        HMODULE hmodule = LoadLibrary(dllpath_str.c_str());
        if (hmodule == nullptr) {
            luaL_errorL(L, "Could not find DLL with name '%s'.", dllpath_str.c_str());
            return 0;
        }
        dllmodules.emplace(dllpath, hmodule);
    }
    HMODULE hmodule = dllmodules[dllpath];
    FARPROC proc = GetProcAddress(hmodule, symbol);
    if (proc == NULL) {
        luaL_errorL(L, "Could not find exported function symbol '%s' in DLL '%s'.", symbol, dllpath.c_str());
        FreeLibrary(hmodule);
        return 0;
    }
    lua_pushcfunction(L, (lua_CFunction)proc, symbol);
    return 1;
#else
    luaL_errorL(L, "dllimport is currently not supported for this platform");
#endif
}
