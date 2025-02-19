#pragma once
#ifdef _WIN32
#include <Windows.h>
#endif
#include <boost/container/flat_map.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <variant>
#include <optional>
#include <dluau.hpp>
struct dluau_Dlmodule {
    using Handle = HMODULE;
    using Fs_path = std::filesystem::path;
    Handle handle;
    std::string name;
    Fs_path path;
    dluau_Dlmodule(Handle handle, std::string name, Fs_path path):
        handle(handle),
        name(std::move(name)),
        path(std::move(path)) {}
    ~dluau_Dlmodule() {if (handle) FreeLibrary(handle);}
    boost::container::flat_map<std::string, uintptr_t> cached{};
    inline static const int tag{dluau_newlightuserdatatag()};
    constexpr static const char* tname{"dlmodule"};
    static void init(lua_State* L);
};

namespace dlimport {
namespace fs = std::filesystem;
using Dlmodule_ref = std::reference_wrapper<dluau_Dlmodule>;
using Dlmodule_map = boost::container::flat_map<fs::path, std::unique_ptr<dluau_Dlmodule>>;
auto get_dlmodules() -> const Dlmodule_map&;
auto find_module(const std::string& name) -> dluau_Dlmodule*;
using Expect_dlmodule = std::expected<Dlmodule_ref, std::string>;
auto init_module(const fs::path& path) -> Expect_dlmodule;
auto load_module(lua_State* L, const std::string& require_path) -> Expect_dlmodule;
auto load_module(lua_State* L) -> Expect_dlmodule;
auto find_proc_address(dluau_Dlmodule& module, const std::string& symbol) -> std::optional<uintptr_t>;
auto lua_tomodule(lua_State* L, int idx) -> dluau_Dlmodule*;
auto lua_pushmodule(lua_State* L, dluau_Dlmodule* module) -> dluau_Dlmodule*;
auto search_path(const fs::path& dlpath) -> std::optional<fs::path>;
inline auto init_require_module(lua_State* L, fs::path path) -> std::expected<void, std::string> {
    path = common::normalize_path(path);
    auto r = init_module(path);
    if (!r) return std::unexpected(r.error());
    dluau_Dlmodule& module = *r;
    auto address = find_proc_address(module, "dlrequire");
    if (not address) return std::unexpected("couldn't find exported 'dlrequire'");
    lua_pushcfunction(L, reinterpret_cast<lua_CFunction>(*address), "dlrequire");
    if (LUA_OK != lua_pcall(L, 0, 1, 0)) {
        std::string errmsg = lua_tostring(L, -1);
        lua_pop(L, 1);
        return std::unexpected(errmsg);
    }
    return std::expected<void, std::string>{};
}
}
