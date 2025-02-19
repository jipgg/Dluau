#include "dlimport.hpp"
#include <filesystem>
#include <functional>
#include <boost/container/flat_set.hpp>
#include <common.hpp>
using namespace std::string_literals;
using boost::container::flat_map;
using std::string, std::unexpected, std::array;
static const array dl_file_extensions = {".so"s, ".dll"s, ".dylib"s};
static boost::container::flat_set<std::filesystem::path> added_dll_directories; 
static dlimport::Dlmodule_map dlmodules;

namespace dlimport {
const Dlmodule_map& get_dlmodules() {
    return dlmodules;
}
auto load_module(lua_State* L, const std::string& require_path) -> Expect_dlmodule {
    if (not dluau::has_permissions(L)) return unexpected("current environment context does not allow loading");
    auto resolved = dluau::resolve_require_path(L, require_path, dl_file_extensions);
    if (!resolved) return unexpected(resolved.error());
    return init_module(*resolved);
}
auto load_module(lua_State* L) -> Expect_dlmodule {
    return load_module(L, luaL_checkstring(L, 1));
}
auto search_path(const fs::path& dlpath) -> std::optional<fs::path> {
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
string get_last_error_as_string() {
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer,
        0,
        nullptr
    );
    string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
}
auto init_module(const fs::path& path) -> Expect_dlmodule {
    if (auto it = dlmodules.find(path); it == dlmodules.end()) {
        if (not added_dll_directories.contains(path)) {
        auto cookie = AddDllDirectory(path.parent_path().c_str());
        if (not cookie) return unexpected(format(
            "failed to add dll directory '{}'\nmessage: {}",
            path.parent_path().string(),
            get_last_error_as_string()));
        } else {
            added_dll_directories.emplace(path);
        }
        HMODULE hm = LoadLibraryEx(
            path.string().c_str(),
            nullptr,
            LOAD_LIBRARY_SEARCH_USER_DIRS |
            LOAD_LIBRARY_SEARCH_DEFAULT_DIRS
        );
        if (not hm) return unexpected(format(
            "failed to load library '{}'\nmessage: {}",
            path.string(),
            get_last_error_as_string()
        ));
        dlmodules.emplace(path, std::make_unique<dluau_Dlmodule>(hm, path.stem().string(), path));
    }
    return *dlmodules.at(path);
}
auto find_proc_address(dluau_Dlmodule& module, const string& symbol) -> std::optional<uintptr_t> {
    auto& cached = module.cached;
    if (auto found = cached.find(symbol); found != cached.end()) return cached.at(symbol);
    FARPROC proc = GetProcAddress(module.handle, symbol.c_str());
    if (proc) cached.emplace(symbol, reinterpret_cast<uintptr_t>(proc));
    return proc ? std::make_optional(reinterpret_cast<uintptr_t>(proc)) : std::nullopt;
}
auto lua_tomodule(lua_State* L, int idx) -> dluau_Dlmodule* {
    if (lua_lightuserdatatag(L, idx) != dluau_Dlmodule::tag) {
        luaL_typeerrorL(L, idx, dluau_Dlmodule::tname);
    }
    auto mod = static_cast<dluau_Dlmodule*>(lua_tolightuserdatatagged(L, idx, dluau_Dlmodule::tag));
    return mod;
}
auto lua_pushmodule(lua_State* L, dluau_Dlmodule* module) -> dluau_Dlmodule* {
    lua_pushlightuserdatatagged(L, module, dluau_Dlmodule::tag);
    luaL_getmetatable(L, dluau_Dlmodule::tname);
    lua_setmetatable(L, -2);
    return module;
}
}
