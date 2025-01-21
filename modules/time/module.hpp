#pragma once
#include <chrono>
#include <goluau.h>
#include <lualib.h>
namespace ch = std::chrono;
using Duration = ch::nanoseconds;
using Time = ch::time_point<ch::system_clock, ch::milliseconds>;
using PreciseTime = ch::time_point<ch::system_clock, Duration>;
constexpr const char* module_name = "time";

Time* to_time(lua_State* L, int idx);
Time& new_time(lua_State* L, const Time& v);
PreciseTime& to_high_precision_time(lua_State* L, int idx);
PreciseTime& new_high_precision_time(lua_State* L, const PreciseTime& v);
Duration* to_duration(lua_State* L, int idx);
Duration& new_duration(lua_State* L, const Duration& v);
