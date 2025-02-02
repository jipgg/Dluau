#pragma once
#ifdef _WIN32
#include <Windows.h>
#endif
#include <boost/container/flat_map.hpp>
#include <dyncall.h>
#include <filesystem>
#include <memory>
#include <string>
#include <variant>
#include <common/error_trail.hpp>
#include <optional>
#include <dluau.h>

namespace dlimport {
struct dlmodule {
    using alias = HMODULE;
    alias handle;
    std::string name;
    std::filesystem::path path;
    dlmodule(alias handle, std::string name, std::filesystem::path path):
        handle(handle),
        name(std::move(name)),
        path(std::move(path)) {}
    ~dlmodule() {if (handle) FreeLibrary(handle);}
    boost::container::flat_map<std::string, uintptr_t> cached{};
    inline static const int tag{dluau_newlightuserdatatag()};
    constexpr static const char* tname{"dlmodule"};
    static int create_binding(lua_State* L);
    static void init(lua_State* L);
};
using dlmodule_map = boost::container::flat_map<std::filesystem::path, std::unique_ptr<dlmodule>>;
const dlmodule_map& get_dlmodules();
using dlmodule_ref = std::reference_wrapper<dlmodule>;
dlmodule* find_module(const std::string& name);
std::variant<dlmodule_ref, common::error_trail> init_module(const std::filesystem::path& path);
std::variant<dlmodule_ref, common::error_trail> load_module(lua_State* L);
std::optional<uintptr_t> find_proc_address(dlmodule& module, const std::string& symbol);
dlmodule* lua_tomodule(lua_State* L, int idx);
dlmodule* lua_pushmodule(lua_State* L, dlmodule* module);
std::optional<std::filesystem::path> search_path(const std::filesystem::path& dlpath);
}
