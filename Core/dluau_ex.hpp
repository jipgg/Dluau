#pragma once
#include <dluau.h>
#include <regex>
#include <expected>
#include <optional>
#include <chrono>
#include <span>
#include <set>
#include <print>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <unordered_map>
#include <ranges>
#include <bitset>
#include <filesystem>
#include <deque>
#include <common.hpp>
#include <variant>
#include <memory>
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
namespace dluau {
using boost::container::flat_map;
using boost::container::flat_set;
using namespace std::string_literals;
using std::string, std::string_view;
using std::span, std::array;
using std::expected, std::unexpected;
using std::format_string, std::format;
namespace fs = std::filesystem;
using Dlmodule = dluau_Dlmodule;
using Dlmodule_ref = std::reference_wrapper<dluau_Dlmodule>;
using Dlmodule_map = boost::container::flat_map<fs::path, std::unique_ptr<dluau_Dlmodule>>;
using Expect_dlmodule = std::expected<Dlmodule_ref, std::string>;
struct Preprocessed_script {
    fs::path normalized_path;
    string identifier;
    string source;
    std::vector<std::string> depends_on_std;
    std::vector<std::string> depends_on_scripts;
    std::vector<std::string> depends_on_dls;
};
using Preprocessed_module_scripts = std::unordered_map<std::string, dluau::Preprocessed_script>;
extern lua_CompileOptions* compile_options;
static const array def_file_exts = {".luau"s, ".lua"s};
constexpr char arg_separator{','};
inline string_view args;
auto get_dlmodules() -> const Dlmodule_map&;
auto find_module(const std::string& name) -> dluau_Dlmodule*;
auto init_dlmodule(const fs::path& path) -> Expect_dlmodule;
auto dlload(lua_State* L, const std::string& require_path) -> Expect_dlmodule;
auto dlload(lua_State* L) -> Expect_dlmodule;
auto find_dlmodule_proc_address(dluau_Dlmodule& module, const std::string& symbol) -> std::optional<uintptr_t>;
auto to_dlmodule(lua_State* L, int idx) -> dluau_Dlmodule*;
auto push_dlmodule(lua_State* L, dluau_Dlmodule* module) -> dluau_Dlmodule*;
auto search_path(const fs::path& dlpath) -> std::optional<fs::path>;
void open_task_library(lua_State* L);
auto load_as_module_script(lua_State* L, const string& file_path, const Preprocessed_module_scripts& modules) -> expected<void, string>;
auto get_preprocessed_modules() -> const std::unordered_map<std::string, Preprocessed_script>&;
auto expand_require_specifiers(
    string& source,
    const fs::path& path,
    string_view fname = "require"
) -> std::vector<std::string>;
auto preprocess_script(const fs::path& path) -> expected<Preprocessed_script, string>;
auto require(lua_State* L, std::string_view name) -> int;
auto get_aliases() -> const flat_map<string, string>&;
auto resolve_require_path(lua_State* L, string name, span<const string> file_exts = def_file_exts) -> expected<string, string>;
auto resolve_require_path(const fs::path& base, string name, span<const string> file_exts = def_file_exts) -> expected<string, string>;
auto load_file(lua_State* L, string_view path) -> expected<lua_State*, string>;
auto load_file(lua_State* L, const Preprocessed_script& pf) -> expected<lua_State*, string>;
auto run_file(lua_State* L, const Preprocessed_script& pf) -> expected<void, string>;
auto run_file(lua_State* L, string_view script_path) -> expected<void, string>;
auto tasks_in_progress() -> bool;
auto task_step(lua_State* L) -> expected<void, string>;
auto has_permissions(lua_State* L) -> bool;
auto default_useratom(const char* key, size_t len) -> int16_t;

inline auto get_precompiled_script_library_values(const fs::path& p) -> decltype(auto) {
    auto as_string_literal = [](const fs::path& path) {
        auto str = path.string();
        std::ranges::replace(str, '\\', '/');
        return format("(\"{}\")", str);
    };
    const auto arr = std::to_array<std::pair<std::regex, string>>({
        {std::regex(R"(\bscript.directory\b)"), as_string_literal(p.parent_path())},
        {std::regex(R"(\bscript.path\b)"), as_string_literal(p)},
        {std::regex(R"(\bscript.name\b)"), as_string_literal(fs::path(p).stem())},
    });
    return arr;
}
inline auto init_require_module(lua_State* L, fs::path path) -> std::expected<void, std::string> {
    path = common::normalize_path(path);
    auto r = init_dlmodule(path);
    if (!r) return std::unexpected(r.error());
    dluau_Dlmodule& module = *r;
    auto address = find_dlmodule_proc_address(module, "dlrequire");
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
