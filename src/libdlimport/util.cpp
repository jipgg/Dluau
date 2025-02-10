#include "dlimport.hpp"
#include <filesystem>
#include <functional>
#include <common.hpp>

namespace dlimport {
static Dlmodule_map dlmodules;
const Dlmodule_map& get_dlmodules() {
    return dlmodules;
}
static const auto dl_file_extensions = std::to_array<String>({".so", ".dll", ".dylib"});
Expected<Ref<Dlmodule>> load_module(lua_State* L) {
    if (not dluau::has_permissions(L)) return Unexpected("current environment context does not allow loading");
    const String name = luaL_checkstring(L, 1);
    auto resolved = dluau::resolve_require_path(L, name, dl_file_extensions);
    if (!resolved) return Unexpected(resolved.error());
    return init_module(*resolved);
}
Opt<Path> search_path(const Path& dlpath) {
    char buffer[MAX_PATH];
    const String dlname = dlpath.string();
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
    String path{buffer};
    return common::normalize_path(path);
}
String get_last_error_as_string() {
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0) {
        return "No error"; // No error occurred
    }

    LPSTR messageBuffer = nullptr;

    // Get the error message from the system
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        errorMessageID,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer,
        0,
        nullptr
    );

    std::string message(messageBuffer, size);

    // Free the buffer allocated by FormatMessage
    LocalFree(messageBuffer);

    return message;
}
Expected<Ref<Dlmodule>> init_module(const fs::path& path) {
    if (auto it = dlmodules.find(path); it == dlmodules.end()) {
        auto cookie = AddDllDirectory(path.parent_path().c_str());
        if (not cookie) return Unexpected(format(
            "failed to add dll directory '{}'\nmessage: {}",
            path.parent_path().string(),
            get_last_error_as_string()
        ));
        HMODULE hm = LoadLibraryEx(
            path.string().c_str(),
            nullptr,
            LOAD_LIBRARY_SEARCH_USER_DIRS |
            LOAD_LIBRARY_SEARCH_DEFAULT_DIRS
        );
        if (not hm) return Unexpected(format(
            "failed to load library '{}'\nmessage: {}",
            path.string(),
            get_last_error_as_string()
        ));
        dlmodules.emplace(path, make_unique<Dlmodule>(hm, path.stem().string(), path));
    }
    return *dlmodules.at(path);
}
Opt<uintptr_t> find_proc_address(Dlmodule& module, const String& symbol) {
    auto& cached = module.cached;
    if (auto found = cached.find(symbol); found != cached.end()) return cached.at(symbol);
    FARPROC proc = GetProcAddress(module.handle, symbol.c_str());
    if (proc) cached.emplace(symbol, reinterpret_cast<uintptr_t>(proc));
    return proc ? std::make_optional(reinterpret_cast<uintptr_t>(proc)) : std::nullopt;
}
Dlmodule* lua_tomodule(lua_State* L, int idx) {
    if (lua_lightuserdatatag(L, idx) != Dlmodule::tag) {
        luaL_typeerrorL(L, idx, Dlmodule::tname);
    }
    auto mod = static_cast<Dlmodule*>(lua_tolightuserdatatagged(L, idx, Dlmodule::tag));
    return mod;
}
Dlmodule* lua_pushmodule(lua_State* L, Dlmodule* module) {
    lua_pushlightuserdatatagged(L, module, Dlmodule::tag);
    luaL_getmetatable(L, Dlmodule::tname);
    lua_setmetatable(L, -2);
    return module;
}
}
