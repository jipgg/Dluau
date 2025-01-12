#pragma once
#include <chrono>
#include <lumin.h>
#include <lualib.h>
namespace ch = std::chrono;
using duration = ch::nanoseconds;
using time_ = ch::time_point<ch::system_clock, ch::milliseconds>;
using high_precision_time = ch::time_point<ch::system_clock, duration>;
constexpr const char* module_name = "date_time";

time_* to_time(lua_State* L, int idx);
time_& new_time(lua_State* L, const time_& v);
high_precision_time& to_high_precision_time(lua_State* L, int idx);
high_precision_time& new_high_precision_time(lua_State* L, const high_precision_time& v);
duration* to_duration(lua_State* L, int idx);
duration& new_duration(lua_State* L, const duration& v);
