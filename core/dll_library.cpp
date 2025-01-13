#include "lumin.h"
#include <lualib.h>
#include <unordered_map>
#include <string>
#include <lualib.h>
#include <luacode.h>
#include <Luau/Common.h>
#include <memory>
#include <cstdint>
#include <optional>
#include <ranges>
#include <algorithm>
#include <Windows.h>
#include <cassert>
#include <iostream>
#include <format>
static int lutag{};
struct Module {
    using Handle = HMODULE;
    Handle handle;
    std::string name;
    std::string path;
    ~Module() {if (handle) FreeLibrary(handle);}
    std::unordered_map<std::string, uintptr_t> cached{};
};
static std::unordered_map<std::string, std::unique_ptr<Module>> loaded_modules{};
std::optional<std::string> find_module_path(const std::string& dllname) {
    char buffer[MAX_PATH];
    DWORD result = SearchPath(
        nullptr,       // Search in standard locations
        dllname.c_str(), // Name of the DLL
        nullptr,       // File extension (optional)
        MAX_PATH,      // Buffer size
        buffer,        // Buffer to store the result
        nullptr        // Ignored (no filename extension needed)
    );
    if (result == 0 or result > MAX_PATH) {
        if (dllname.ends_with(".dll")) return std::nullopt;
        return find_module_path(dllname + ".dll");
    }
    std::string path{buffer};
    std::ranges::replace(path, '\\', '/');
    return path;
}
static Module* init_or_find_module(const std::string& name) {
    auto found_path = find_module_path(name);
    if (not found_path) return nullptr;
    if (auto it = loaded_modules.find(*found_path); it == loaded_modules.end()) {
        HMODULE hm = LoadLibrary(found_path->c_str());
        if (not hm) [[unlikely]] return nullptr;
        loaded_modules.emplace(*found_path, std::make_unique<Module>(hm, name, *found_path));
    }
    return loaded_modules[*found_path].get();
}
static std::optional<uintptr_t> find_proc_address(Module& module, const std::string& symbol) {
    auto& cached = module.cached;
    if (auto found = cached.find(symbol); found != cached.end()) return cached.at(symbol);
    FARPROC proc = GetProcAddress(module.handle, symbol.c_str());
    if (proc) cached.emplace(symbol, reinterpret_cast<uintptr_t>(proc));
    return proc ? std::make_optional(reinterpret_cast<uintptr_t>(proc)) : std::nullopt;
}
static Module* lua_tomodule(lua_State* L, int idx) {
    if (lua_lightuserdatatag(L, idx) != lutag) luaL_typeerrorL(L, idx, "module");
    auto mod = static_cast<Module*>(lua_tolightuserdatatagged(L, idx, lutag));
    std::cout << mod->name << " " << mod->path << "\n";
    return mod;
}
static int findpath(lua_State* L) {
    if (auto path = find_module_path(luaL_checkstring(L, 1))) {
        lua_pushstring(L, path->c_str());
        return 1;
    }
    return 0;
}
static int getmodule(lua_State* L) {
    if (auto path = find_module_path(luaL_checkstring(L, 1))) {
        Module* module = init_or_find_module(*path);
        lua_pushlightuserdatatagged(L, module, lutag);
        return 1;
    }
    luaL_argerrorL(L, 1, "couldn't find dll");
}
static int getfunc(lua_State* L) {
    const char* proc_key = luaL_checkstring(L, 2);
    auto opt = find_proc_address(*lua_tomodule(L, 1), proc_key);
    if (not opt) luaL_errorL(L, "function was not found ");
    const auto fmt = std::format("[dllimport]{}", proc_key);
    lua_pushcfunction(L, reinterpret_cast<lua_CFunction>(*opt), fmt.c_str());
    return 1;
}
static int findmodule(lua_State* L) {
    if (auto path = find_module_path(luaL_checkstring(L, 1))) {
        auto module = init_or_find_module(*path);
        lua_pushlightuserdatatagged(L, &module, lutag);
        return 1;
    }
    return 0;
}
// kind of unsafe
static int unload(lua_State* L) {
    if (auto path = find_module_path(luaL_checkstring(L, 1))) {
        auto found_it = loaded_modules.find(*path);
        if (found_it == loaded_modules.end()) return 0;
        loaded_modules.erase(*path);
    }
    return 0;
}
static int loaded(lua_State* L) {
    lua_newtable(L);
    int i{1};
    for (const auto& [key, v] : loaded_modules) {
        lua_pushstring(L, key.c_str());
        lua_rawseti(L, -2, i++);
    }
    return 1;
}
void luminopen_dll(lua_State* L) {
    lutag = lumin_newlutag();
    lua_setlightuserdataname(L, lutag, "module");
    const luaL_Reg lib[] = {
        {"getmodule", getmodule},
        {"findmodule", findmodule},
        {"getfunction", getfunc},
        {"findpath", findpath},
        {"loaded", loaded},
        {nullptr, nullptr}
    };
    luaL_register(L, "dll", lib);
}
