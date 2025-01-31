#pragma once
#include <boost/container/flat_map.hpp>
#include <lualib.h>
#include <string>
#include <dluau.h>
#include <string_view>
#include <filesystem>
#include <optional>
#include <variant>
#include <regex>
#include <common/error_trail.hpp>
#include <array>

namespace shared {
using boost::container::flat_map;
using std::variant, std::optional;
using std::string_view, std::string;
using common::error_trail;
using std::pair, std::regex, std::span;
using std::filesystem::path;
extern lua_CompileOptions* compile_options;
const flat_map<lua_State*, string>& get_script_paths();
const flat_map<string, string>& get_aliases();
static const auto default_file_extensions = std::to_array<string>({".luau", ".lua"});
variant<string, error_trail> resolve_path(string name, const path& base, span<const string> file_exts = default_file_extensions);
variant<lua_State*, error_trail> load_file(lua_State* L, string_view path);
optional<error_trail> run_file(lua_State* L, string_view script_path);
bool tasks_in_progress();
optional<error_trail> task_step(lua_State* L);
bool has_permissions(lua_State* L);
constexpr char arg_separator{','};
int16_t default_useratom(const char* key, size_t len);
inline string_view args;
bool precompile(string& source);
bool precompile(string& source, span<const pair<regex, string>> sv);
}
