#pragma once
#include <chrono>
#include <lumin.h>
#include <lualib.h>
namespace ch = std::chrono;
using duration_type = ch::nanoseconds;
using time_type = ch::time_point<ch::system_clock, ch::milliseconds>;
using precise_time_type = ch::time_point<ch::system_clock, duration_type>;
constexpr const char* module_name = "date_time";

time_type* to_time(lua_State* L, int idx);
time_type& new_time(lua_State* L, const time_type& v);
precise_time_type& to_precise_time(lua_State* L, int idx);
precise_time_type& new_precise_time(lua_State* L, const precise_time_type& v);
duration_type* to_duration(lua_State* L, int idx);
duration_type& new_duration(lua_State* L, const duration_type& v);
void register_duration(lua_State* L);
