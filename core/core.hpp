#pragma once
#include <boost/container/flat_map.hpp>
#include <lualib.h>
#include <string>
#include <vector>
#include <string_view>

inline boost::container::flat_map<lua_State*, std::string> script_path_registry;
inline std::string_view process_args;
