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
#include <boost/container/flat_map.hpp>
#include <format>
static int lutag{};
static int getfunc_stringatom{};
static constexpr const char* tname{"module"};
struct Module {
    using Handle = HMODULE;
    Handle handle;
    std::string name;
    std::string path;
    ~Module() {if (handle) FreeLibrary(handle);}
    boost::container::flat_map<std::string, uintptr_t> cached{};
};
static boost::container::flat_map<std::string, std::unique_ptr<Module>> loaded_modules{};
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
    if (lua_lightuserdatatag(L, idx) != lutag) luaL_typeerrorL(L, idx, tname);
    auto mod = static_cast<Module*>(lua_tolightuserdatatagged(L, idx, lutag));
    return mod;
}
static Module* lua_pushmodule(lua_State* L, Module* module) {
    lua_pushlightuserdatatagged(L, module, lutag);
    luaL_getmetatable(L, tname);
    lua_setmetatable(L, -2);
    return module;
}
static int findpath(lua_State* L) {
    if (auto path = find_module_path(luaL_checkstring(L, 1))) {
        lua_pushstring(L, path->c_str());
        return 1;
    }
    return 0;
}
static int getmodule(lua_State* L) {
    Module* module = init_or_find_module(luaL_checkstring(L, 1));
    if (not module) luaL_argerrorL(L, 1, "couldn't find dll");
    lua_pushmodule(L, module);
    return 1;
}
static int getfunc(lua_State* L) {
    const char* proc_key = luaL_checkstring(L, 2);
    auto opt = find_proc_address(*lua_tomodule(L, 1), proc_key);
    if (not opt) luaL_errorL(L, "function was not found ");
    const auto fmt = std::format("{}", proc_key);
    lua_pushcfunction(L, reinterpret_cast<lua_CFunction>(*opt), fmt.c_str());
    return 1;
}
static int findmodule(lua_State* L) {
    auto module = init_or_find_module(luaL_checkstring(L, 1));
    if (not module) return 0;
    lua_pushmodule(L, module);
    return 1;
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
static int module_index(lua_State* L) {
    Module* module = lua_tomodule(L, 1);
    const std::string_view key = luaL_checkstring(L, 2);
    if (key == "path") {
        lua_pushstring(L, module->path.c_str());
        return 1;
    } else if (key == "name") {
        lua_pushstring(L, module->name.c_str());
        return 1;
    }
    luaL_argerrorL(L, 2, "index was null");
}
static int module_namecall(lua_State* L) {
    Module& module = *lua_tomodule(L, 1);
    int atom;
    lua_namecallatom(L, &atom);
    if (atom == getfunc_stringatom) return getfunc(L);
    luaL_errorL(L, "invalid");
}
int luminload_dll(lua_State* L) {
    if (luaL_newmetatable(L, tname)) {
        lutag = lumin_newlightuserdatatag();
        getfunc_stringatom = lumin_stringatom(L, "getfunc");
        lua_setlightuserdataname(L, lutag, tname);
        const luaL_Reg meta[] = {
            {"__index", module_index},
            {"__namecall", module_namecall},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
    }
    lua_pop(L, 1);
    const luaL_Reg lib[] = {
        {"getmodule", getmodule},
        {"findmodule", findmodule},
        {"getfunc", getfunc},
        {"findpath", findpath},
        {"loaded", loaded},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    return 1;
}
void luminopen_dll(lua_State* L) {
    luminload_dll(L);
    lua_setglobal(L, "dll");
}
