#pragma once
#include <boost/container/flat_map.hpp>
#include <lualib.h>
#include <string>
#include <dluau.h>
#include <string_view>
#include <optional>
#include <ErrorInfo.hpp>

constexpr char arg_separator{','};
namespace shared {
extern lua_CompileOptions* compile_options;
int push_metadatatable(lua_State* L);
int16_t default_useratom(const char* key, size_t len);
inline lua_State* main_state;
inline std::string_view args;
inline boost::container::flat_map<lua_State*, std::string> script_paths;
void process_precompiled_features(std::string& source);
namespace script_utils {
std::optional<lua_State*> load(lua_State* L, std::string_view path);
std::optional<ErrorInfo> run(lua_State* L, std::string_view script_path);
}
}
