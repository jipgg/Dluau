#include "dlimport.hpp"
#include <shared.hpp>
#include <filesystem>
#include <functional>
#include <common.hpp>
namespace fs = std::filesystem;
using std::optional, std::string, std::string_view;
using std::variant;
using common::error_trail;

namespace dlimport {
static dlmodule_map dlmodules;
const dlmodule_map& get_dlmodules() {
    return dlmodules;
}
static const auto dl_file_extensions = std::to_array<string>({".so", ".dll", ".dylib"});
variant<dlmodule_ref, error_trail> load_module(lua_State* L) {
    if (not shared::has_permissions(L)) return error_trail("current environment context does not allow loading");
    const string name = luaL_checkstring(L, 1);
    auto resolved = shared::resolve_require_path(L, name, dl_file_extensions);
    if (auto err = std::get_if<error_trail>(&resolved)) return err->propagate();
    auto p = std::get<string>(resolved);
    return std::ref(init_module(p));
}
optional<fs::path> search_path(const fs::path& dlpath) {
    char buffer[MAX_PATH];
    const string dlname = dlpath.string();
    DWORD result = SearchPath(nullptr, dlname.c_str(), nullptr, MAX_PATH, buffer, nullptr);
    if (result == 0 or result > MAX_PATH) {
        if (not dlpath.has_extension()) {
            fs::path path = dlpath;
            for (const auto& ext : dl_file_extensions) {
                path = path.replace_extension(ext);
                auto opt = search_path(path);
                if (opt) return std::move(opt);
            }
        }
        return std::nullopt;
    }
    string path{buffer};
    return common::normalize_path(path);
}
dlmodule& init_module(const fs::path& path) {
    if (auto it = dlmodules.find(path); it == dlmodules.end()) {
        HMODULE hm = LoadLibrary(path.string().c_str());
        dlmodules.emplace(path, make_unique<dlmodule>(hm, path.stem().string(), path));
    }
    return *dlmodules.at(path);
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
