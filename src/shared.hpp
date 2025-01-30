#pragma once
#include <boost/container/flat_map.hpp>
#include <lualib.h>
#include <string>
#include <dluau.h>
#include <string_view>
#include <optional>
#include <variant>
#include <regex>
#include <common/error_trail.hpp>

struct dluau_static_value {
    const char* regex;
    const char* value;
};

namespace shared {
extern lua_CompileOptions* compile_options;
const boost::container::flat_map<lua_State*, std::string>& get_script_paths();
std::variant<lua_State*, common::error_trail> load_file(lua_State* L, std::string_view path);
std::optional<common::error_trail> run_file(lua_State* L, std::string_view script_path);
bool tasks_in_progress();
std::optional<common::error_trail> task_step(lua_State* L);
bool has_permissions(lua_State* L);
constexpr char arg_separator{','};
int16_t default_useratom(const char* key, size_t len);
inline std::string_view args;
bool precompile(std::string& source);
bool precompile(std::string& source, std::span<const std::pair<std::regex, std::string>> sv);
}
