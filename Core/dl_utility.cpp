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
auto dluau_dlfindprocaddress(dluau_Dlmodule* dlm, const char* symbol) -> uintptr_t {
    return dluau::find_dlmodule_proc_address(*dlm, symbol).value_or(0);
}
auto dluau_getdlmodule(const char* require_path) -> dluau_Dlmodule* {
    auto res = dluau::get_dlmodule(common::normalize_path(require_path));
    if (!res) return nullptr;
    return &(res->get());
}
const Dlmodule_map& dluau::get_dlmodules() {
    return loaded_dlmodules;
}
auto dluau::get_dlmodule(const fs::path& require_path) -> Expect_dlmodule {
    if (not loaded_dlmodules.contains(require_path)) {
        return std::unexpected(
            std::format(
                "dynamic dl loading is not allowed. '{}' did not exist.",
                require_path.string()
            )
        );
    }
    return *loaded_dlmodules.at(require_path);
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
auto dluau::init_dlmodule(lua_State* L, const fs::path& path) -> Expect_dlmodule {
    if (not loaded_dlmodules.contains(path)) {
        if (not added_dl_directories.contains(path)) {
            auto cookie = AddDllDirectory(path.parent_path().c_str());
            if (not cookie) {
                constexpr const char* errfmt{"failed to add dl directory '{}'\nmessage: {}"};
                return unexpected(format(errfmt, path.parent_path().string(), get_last_error_as_string()));
            }
            added_dl_directories.emplace(path);
        }
        HMODULE hm = LoadLibraryEx(
            path.string().c_str(),
            nullptr,
            LOAD_LIBRARY_SEARCH_USER_DIRS |
            LOAD_LIBRARY_SEARCH_DEFAULT_DIRS
        );
        if (not hm) {
            constexpr const char* errfmt{"failed to load library '{}'\nmessage: {}"};
            return unexpected(format(errfmt, path.string(), get_last_error_as_string()));
        }
        auto module = std::make_unique<dluau_Dlmodule>(hm, path.stem().string(), path);
        auto found = dluau::find_dlmodule_proc_address(*module, "dlinit");
        if (found) {
            auto dlinit = reinterpret_cast<void(*)(lua_State*)>(*found);
            dlinit(L);
        }
        auto it = loaded_dlmodules.emplace(path, std::move(module));
        return *it.first->second;
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
