#include "lib.hpp"
#include <shared.hpp>
#include <filesystem>
namespace fs = std::filesystem;
namespace rn = std::ranges;
using std::optional, std::string, std::string_view;

namespace util {
optional<string> find_module_path(const string& dllname,  string_view priority_dir) {
    if (not priority_dir.empty()) {
        fs::path potential_path = fs::path(priority_dir) / dllname;
        if (not potential_path.has_extension()) {
            potential_path.replace_extension(".dll");
        }
        auto standardize = [&potential_path]() -> string {
            string standardized = fs::absolute(potential_path).string();
            rn::replace(standardized, '\\', '/');
            return standardized;
        };
        if (not fs::exists(potential_path)) return std::nullopt;
        string path = fs::absolute(potential_path).string();
        rn::replace(path, '\\', '/');
        return path;
    }
    char buffer[MAX_PATH];
    DWORD result = SearchPath(nullptr, dllname.c_str(), nullptr, MAX_PATH, buffer, nullptr);
    if (result == 0 or result > MAX_PATH) {
        if (dllname.ends_with(".dll")) {
            for (const auto& [state, path] : dluau::get_script_paths()) {
                fs::path dllpath = fs::path(path).parent_path() / dllname;
                if (fs::exists(dllpath)) return fs::absolute(dllpath).string();
            }
            return std::nullopt;
        }
        return find_module_path(dllname + ".dll");
    }
    string path{buffer};
    rn::replace(path, '\\', '/');
    return path;
}
dlmodule* init_or_find_module(const string& name, string_view priority_dir) {
    auto found_path = find_module_path(name, priority_dir);
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
