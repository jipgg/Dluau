#include "lumin.h"
#include <lualib.h>
#include <unordered_map>
#include <string>
#include <lualib.h>
#include <luacode.h>
#include <Luau/Common.h>
#include <ranges>
#include "error_info.hpp"
#include <variant>
#include <format>
#include <cassert>
#ifdef _WIN32
#include <Windows.h>
struct dll_module {
    HMODULE module; //is void so cant use custom unique_ptr destructor
    explicit dll_module(HMODULE module): module(module) {}
    dll_module(const dll_module&) = delete;
    dll_module& operator=(const dll_module&) = delete;
    dll_module(dll_module&&) noexcept = delete;
    dll_module& operator=(dll_module&&) noexcept = delete;
    ~dll_module() {if (module) FreeLibrary(module);}
};
#endif
static std::unordered_map<std::string, std::unique_ptr<dll_module>> loaded_dlls{};
static bool is_allowed(lua_State* L) {
    lua_Debug ar;
    if (lua_getinfo(L, 1, "s", &ar)) {
        return ar.source[0] == '@' or ar.source[0] == '=';
    }
    return false;
}
static void check_context(lua_State* L) {
    constexpr const char* err_wrong_context = "The current script enviroment does not allow DLL loading.";
    if (not is_allowed(L)) luaL_errorL(L, err_wrong_context);
}
static std::optional<std::string_view> file_extension(std::string_view filename) {
    auto last = filename.find_last_of('.');
    if (last == filename.npos) return std::nullopt;
    return filename.substr(last);
}
std::optional<std::string> find_dll_path(const std::string& dllname) {
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
        return find_dll_path(dllname + ".dll");
    }
    std::string path{buffer};
    std::ranges::replace(path, '\\', '/');
    return path;
}
// TODO: should search for these dll paths and store them as absolute paths
static std::variant<lua_CFunction, error_info> load_dllfunction(const std::string& path, const std::string& symbol) {
#ifdef _WIN32
    auto found_path = find_dll_path(path);
    if (not found_path) {
        return error_info{std::format("Could not find DLL with name '{}'.", path)};
    }
    if (auto it = loaded_dlls.find(*found_path); it == loaded_dlls.end()) {
        HMODULE hmodule = LoadLibrary(found_path->c_str());
        assert(hmodule);
        loaded_dlls.emplace(*found_path, std::make_unique<dll_module>(hmodule));
    }
    HMODULE hmodule = loaded_dlls[*found_path]->module;
    FARPROC proc = GetProcAddress(hmodule, symbol.c_str());
    if (proc == NULL) {
        return error_info{std::format("Could not find exported function symbol '{}' in DLL '{}'.", symbol, *found_path)};
    }
    return (lua_CFunction)proc;
#endif
    return error_info{"Platform is currently unsupported."};
}
static int loadfunction(lua_State* L) {
    check_context(L);
    const char* path = luaL_checkstring(L, 1);
    const char* function = luaL_checkstring(L, 2);
    auto result = load_dllfunction(path, function);
    if (auto err = std::get_if<error_info>(&result)) {
        luaL_errorL(L, err->message().c_str());
        return 0;
    }
    lua_pushcfunction(L, std::get<lua_CFunction>(result), std::format("imported '{}' from '{}'", function, path).c_str());
    return 1;
}
static int loadmodule(lua_State* L) {
    check_context(L);
    const char* path = luaL_checkstring(L, 1);
    auto result = load_dllfunction(path, "loadmodule");
    if (auto err = std::get_if<error_info>(&result)) {
        luaL_errorL(L, err->message().c_str());
        return 0;
    }
    return std::get<lua_CFunction>(result)(L);
}
static int findpath(lua_State* L) {
    check_context(L);
    if (auto path = find_dll_path(luaL_checkstring(L, 1))) {
        lua_pushstring(L, path->c_str());
        return 1;
    }
    return 0;
}
// kind of unsafe
static int unload(lua_State* L) {
    check_context(L);
    if (auto path = find_dll_path(luaL_checkstring(L, 1))) {
        auto found_it = loaded_dlls.find(*path);
        if (found_it == loaded_dlls.end()) return 0;
        loaded_dlls.erase(*path);
    }
    return 0;
}
static int loaded(lua_State* L) {
    lua_newtable(L);
    int i{1};
    for (const auto& [key, v] : loaded_dlls) {
        lua_pushstring(L, key.c_str());
        lua_rawseti(L, -2, i++);
    }
    return 1;
}
void luminopen_dll(lua_State* L) {
    const luaL_Reg libdll[] = {
        {"loadfunction", loadfunction},
        {"loadmodule", loadmodule},
        {"findpath", findpath},
        //{"unload", unload},
        {"loaded", loaded},
        {nullptr, nullptr}
    };
    luaL_register(L, "dll", libdll);
}
