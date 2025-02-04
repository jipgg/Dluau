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
using aggregate = std::unique_ptr<DCaggr, decltype(&dcFreeAggr)>;
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
    boost::container::flat_map<std::string, aggregate> aggregates{};
    inline static const int tag{dluau_newlightuserdatatag()};
    constexpr static const char* tname{"dlmodule"};
    static int create_binding(lua_State* L);
    static int create_aggregate(lua_State* L);
    static void init(lua_State* L);
};
enum class c_type {
    c_int,
    c_uint,
    c_ulong,
    c_long,
    c_ushort,
    c_char,
    c_uchar,
    c_short,
    c_void,
    c_float,
    c_double,
    c_void_ptr,
    c_char_ptr,
    c_bool,
};
std::optional<c_type> string_to_param_type(std::string_view str);

using dlmodule_map = boost::container::flat_map<std::filesystem::path, std::unique_ptr<dlmodule>>;
const dlmodule_map& get_dlmodules();
using dlmodule_ref = std::reference_wrapper<dlmodule>;
void push_c_types(lua_State* L);
dlmodule* find_module(const std::string& name);
std::variant<dlmodule_ref, common::error_trail> init_module(const std::filesystem::path& path);
std::variant<dlmodule_ref, common::error_trail> load_module(lua_State* L);
std::optional<uintptr_t> find_proc_address(dlmodule& module, const std::string& symbol);
dlmodule* lua_tomodule(lua_State* L, int idx);
dlmodule* lua_pushmodule(lua_State* L, dlmodule* module);
std::optional<std::filesystem::path> search_path(const std::filesystem::path& dlpath);
}
