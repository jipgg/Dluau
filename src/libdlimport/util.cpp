#include "local.hpp"
#include <shared.hpp>
#include <filesystem>
#include <common.hpp>
namespace fs = std::filesystem;
namespace rn = std::ranges;
using std::optional, std::string, std::string_view;

namespace util {
optional<string> search_path(const string& dlname) {
    char buffer[MAX_PATH];
    DWORD result = SearchPath(nullptr, dlname.c_str(), nullptr, MAX_PATH, buffer, nullptr);
    if (result == 0 or result > MAX_PATH) {
        return search_path(dlname + ".dll");
    }
    string path{buffer};
    return common::make_path_pretty(common::sanitize_path(path));
}
optional<string> find_module_path(const string& dllname) {
    std::string path{dllname};
    path = common::substitute_user_folder(path).value_or(path);
    if (not fs::exists(path)) return std::nullopt;
    return common::sanitize_path(path);
}
dlmodule& init_module(const string& path) {
    if (auto it = glob::loaded.find(path); it == glob::loaded.end()) {
        HMODULE hm = LoadLibrary(path.c_str());
        glob::loaded.emplace(path, make_unique<dlmodule>(hm, fs::path(path).stem().string(), path));
    }
    return *glob::loaded[path];
}
dlmodule* init_or_find_module(const string& name) {
    auto found_path = find_module_path(name);
    if (not found_path) return nullptr;
    if (auto it = glob::loaded.find(*found_path); it == glob::loaded.end()) {
        HMODULE hm = LoadLibrary(found_path->c_str());
        if (not hm) [[unlikely]] return nullptr;
        glob::loaded.emplace(*found_path, make_unique<dlmodule>(hm, name, *found_path));
    }
    return glob::loaded[*found_path].get();
}
optional<uintptr_t> find_proc_address(dlmodule& module, const string& symbol) {
    auto& cached = module.cached;
    if (auto found = cached.find(symbol); found != cached.end()) return cached.at(symbol);
    FARPROC proc = GetProcAddress(module.handle, symbol.c_str());
    if (proc) cached.emplace(symbol, reinterpret_cast<uintptr_t>(proc));
    return proc ? std::make_optional(reinterpret_cast<uintptr_t>(proc)) : std::nullopt;
}
dlmodule* lua_tomodule(lua_State* L, int idx) {
    if (lua_lightuserdatatag(L, idx) != dlmodule::tag) {
        luaL_typeerrorL(L, idx, dlmodule::tname);
    }
    auto mod = static_cast<dlmodule*>(lua_tolightuserdatatagged(L, idx, dlmodule::tag));
    return mod;
}
dlmodule* lua_pushmodule(lua_State* L, dlmodule* module) {
    lua_pushlightuserdatatagged(L, module, dlmodule::tag);
    luaL_getmetatable(L, dlmodule::tname);
    lua_setmetatable(L, -2);
    return module;
}
}
