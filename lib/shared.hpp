#pragma once
#include <boost/container/flat_map.hpp>
#include <lualib.h>
#include <string>
#include <dluau.h>
#include <string_view>
#include <optional>
#include <common/error_trail.hpp>

namespace dluau {
extern lua_CompileOptions* compile_options;
const boost::container::flat_map<lua_State*, std::string>& get_script_paths();
std::optional<lua_State*> load_file(lua_State* L, std::string_view path);
std::optional<common::error_trail> run_file(lua_State* L, std::string_view script_path);
bool has_permissions(lua_State* L);
constexpr char arg_separator{','};
int16_t default_useratom(const char* key, size_t len);
int push_metadatatable(lua_State* L);
int push_print(lua_State* L);
int push_scan(lua_State* L);
int require(lua_State* L);
inline std::string_view args;
bool precompile(std::string& source);
}
