#pragma once
#include <chrono>
#include <lumin.h>
#include <lualib.h>
namespace ch = std::chrono;
using Duration = ch::nanoseconds;
using Time = ch::time_point<ch::system_clock, ch::milliseconds>;
using High_precision_time = ch::time_point<ch::system_clock, Duration>;
constexpr const char* module_name = "time";

Time* to_time(lua_State* L, int idx);
Time& new_time(lua_State* L, const Time& v);
High_precision_time& to_high_precision_time(lua_State* L, int idx);
High_precision_time& new_high_precision_time(lua_State* L, const High_precision_time& v);
Duration* to_duration(lua_State* L, int idx);
Duration& new_duration(lua_State* L, const Duration& v);
