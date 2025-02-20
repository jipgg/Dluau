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
namespace dluau {
using boost::container::flat_map;
using boost::container::flat_set;
using namespace std::string_literals;
using std::string, std::string_view;
using std::span, std::array;
using std::expected, std::unexpected;
using std::format_string, std::format;
namespace fs = std::filesystem;
struct Preprocessed_file {
    fs::path normalized_path;
    string identifier;
    string source;
    std::vector<std::string> depends_on_std;
    std::vector<std::string> depends_on_scripts;
};
extern lua_CompileOptions* compile_options;
static const array def_file_exts = {".luau"s, ".lua"s};
constexpr char arg_separator{','};
inline string_view args;
void open_task_library(lua_State* L);
void open_dlimport_library(lua_State* L);
auto get_preprocessed_modules() -> const std::unordered_map<std::string, Preprocessed_file>&;
auto expand_require_specifiers(
    string& source,
    const fs::path& path
) -> std::vector<std::string>;
auto preprocess_source(const fs::path& path) -> expected<Preprocessed_file, string>;
auto get_script_paths() -> flat_map<lua_State*, string>&;
auto get_aliases() -> const flat_map<string, string>&;
auto resolve_require_path(lua_State* L, string name, span<const string> file_exts = def_file_exts) -> expected<string, string>;
auto resolve_require_path(const fs::path& base, string name, span<const string> file_exts = def_file_exts) -> expected<string, string>;
auto resolve_path(string name, const fs::path& base, span<const string> = def_file_exts) -> expected<string, string>;
auto load_file(lua_State* L, string_view path) -> expected<lua_State*, string>;
auto load_file(lua_State* L, const Preprocessed_file& pf) -> expected<lua_State*, string>;
auto run_file(lua_State* L, const Preprocessed_file& pf) -> expected<void, string>;
auto run_file(lua_State* L, string_view script_path) -> expected<void, string>;
auto tasks_in_progress() -> bool;
auto task_step(lua_State* L) -> expected<void, string>;
auto has_permissions(lua_State* L) -> bool;
auto default_useratom(const char* key, size_t len) -> int16_t;
auto precompile(string& source, span<const std::pair<std::regex, string>> sv) -> bool;

inline auto get_precompiled_library_values(const fs::path& p) -> decltype(auto) {
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
}
