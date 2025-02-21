#include <dluau.hpp>
#include <filesystem>
#include <functional>
#include <boost/container/flat_set.hpp>
#include <common.hpp>
using namespace std::string_literals;
using boost::container::flat_map;
using boost::container::flat_set;
namespace fs = std::filesystem;
using std::string, std::unexpected, std::array;
static const array dl_file_extensions = {".so"s, ".dll"s, ".dylib"s};
static boost::container::flat_set<std::filesystem::path> added_dl_directories; 
using dluau::Dlmodule_map, dluau::Expect_dlmodule;
static dluau::Dlmodule_map loaded_dlmodules;

auto dluau_todlmodule(lua_State* L, int idx) -> dluau_Dlmodule* {
    return dluau::to_dlmodule(L, idx);
}
void dluau_pushdlmodule(lua_State* L, dluau_Dlmodule* dlm) {
    dluau::push_dlmodule(L, dlm);
}
auto dluau_dlmodulefind(dluau_Dlmodule* dlm, const char* symbol) -> uintptr_t {
    return dluau::find_dlmodule_proc_address(*dlm, symbol).value_or(0);
}
auto dluau_loaddlmodule(lua_State* L, const char* require_path) -> dluau_Dlmodule* {
    auto res = dluau::dlload(L, require_path);
    if (!res) return nullptr;
    return &(res->get());
}
const Dlmodule_map& dluau::get_dlmodules() {
    return loaded_dlmodules;
}
auto dluau::dlload(lua_State* L, const std::string& require_path) -> Expect_dlmodule {
    if (not loaded_dlmodules.contains(require_path)) {
        return std::unexpected(std::format("Dynamic dl loading is not allowed. '{}' did not exist.", require_path));
    }
    return *loaded_dlmodules.at(require_path);
}
auto dluau::dlload(lua_State* L) -> Expect_dlmodule {
    return dlload(L, luaL_checkstring(L, 1));
}
auto dluau::search_path(const fs::path& dlpath) -> std::optional<fs::path> {
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
static auto get_last_error_as_string() -> string {
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
auto dluau::init_dlmodule(const fs::path& path) -> Expect_dlmodule {
    if (not loaded_dlmodules.contains(path)) {
        if (not added_dl_directories.contains(path)) {
        auto cookie = AddDllDirectory(path.parent_path().c_str());
        if (not cookie) return unexpected(format(
            "failed to add dll directory '{}'\nmessage: {}",
            path.parent_path().string(),
            get_last_error_as_string()));
        } else {
            added_dl_directories.emplace(path);
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
        loaded_dlmodules.emplace(path, std::make_unique<dluau_Dlmodule>(hm, path.stem().string(), path));
    }
    return *loaded_dlmodules.at(path);
}
auto dluau::find_dlmodule_proc_address(dluau_Dlmodule& module, const string& symbol) -> std::optional<uintptr_t> {
    auto& cached = module.cached;
    if (auto found = cached.find(symbol); found != cached.end()) return cached.at(symbol);
    FARPROC proc = GetProcAddress(module.handle, symbol.c_str());
    if (proc) cached.emplace(symbol, reinterpret_cast<uintptr_t>(proc));
    return proc ? std::make_optional(reinterpret_cast<uintptr_t>(proc)) : std::nullopt;
}
auto dluau::to_dlmodule(lua_State* L, int idx) -> dluau_Dlmodule* {
    if (lua_lightuserdatatag(L, idx) != dluau_Dlmodule::tag) {
        luaL_typeerrorL(L, idx, dluau_Dlmodule::tname);
    }
    auto mod = static_cast<dluau_Dlmodule*>(lua_tolightuserdatatagged(L, idx, dluau_Dlmodule::tag));
    return mod;
}
auto dluau::push_dlmodule(lua_State* L, dluau_Dlmodule* module) -> dluau_Dlmodule* {
    lua_pushlightuserdatatagged(L, module, dluau_Dlmodule::tag);
    luaL_getmetatable(L, dluau_Dlmodule::tname);
    lua_setmetatable(L, -2);
    return module;
}
