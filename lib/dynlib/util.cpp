#include "dynlib.hpp"
#include <core.hpp>
#include <goluau.h>
#include <filesystem>
#include <ranges>
namespace fs = std::filesystem;
namespace rn = std::ranges;

namespace util {
Opt<String> find_module_path(const String& dllname) {
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
        if (dllname.ends_with(".dll")) {
            for (const auto& [state, path] : script_path_registry) {
                fs::path dllpath = fs::path(path).parent_path() / dllname;
                if (fs::exists(dllpath)) return fs::absolute(dllpath).string();
            }
            return std::nullopt;
        }
        return find_module_path(dllname + ".dll");
    }
    String path{buffer};
    rn::replace(path, '\\', '/');
    return path;
}
Dllmodule* init_or_find_module(const String& name) {
    auto found_path = find_module_path(name);
    if (not found_path) return nullptr;
    if (auto it = glob::loaded.find(*found_path); it == glob::loaded.end()) {
        HMODULE hm = LoadLibrary(found_path->c_str());
        if (not hm) [[unlikely]] return nullptr;
        glob::loaded.emplace(*found_path, make_unique<Dllmodule>(hm, name, *found_path));
    }
    return glob::loaded[*found_path].get();
}
Opt<uintptr_t> find_proc_address(Dllmodule& module, const String& symbol) {
    auto& cached = module.cached;
    if (auto found = cached.find(symbol); found != cached.end()) return cached.at(symbol);
    FARPROC proc = GetProcAddress(module.handle, symbol.c_str());
    if (proc) cached.emplace(symbol, reinterpret_cast<uintptr_t>(proc));
    return proc ? std::make_optional(reinterpret_cast<uintptr_t>(proc)) : std::nullopt;
}
Dllmodule* lua_tomodule(lua_State* L, int idx) {
    if (lua_lightuserdatatag(L, idx) != Dllmodule::tag) {
        luaL_typeerrorL(L, idx, Dllmodule::tname);
    }
    auto mod = static_cast<Dllmodule*>(lua_tolightuserdatatagged(L, idx, Dllmodule::tag));
    return mod;
}
Dllmodule* lua_pushmodule(lua_State* L, Dllmodule* module) {
    lua_pushlightuserdatatagged(L, module, Dllmodule::tag);
    luaL_getmetatable(L, Dllmodule::tname);
    lua_setmetatable(L, -2);
    return module;
}
}
