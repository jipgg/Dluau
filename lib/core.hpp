#pragma once
#include <boost/container/flat_map.hpp>
#include <lualib.h>
#include <string>
#include <vector>
#include <string_view>
#include <optional>
#include <ErrorInfo.hpp>

inline boost::container::flat_map<lua_State*, std::string> script_path_registry;
inline std::string_view launch_args;
constexpr char arg_separator{','};
inline lua_State* main_state;
namespace script_utils {
std::optional<lua_State*> load(lua_State* L, std::string_view path);
std::optional<ErrorInfo> run(lua_State* L, std::string_view script_path);
}
