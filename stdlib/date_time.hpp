#pragma once
#include <chrono>
#include <lumin.h>
#include <lualib.h>
namespace ch = std::chrono;
using duration = ch::nanoseconds;
using time_point = ch::time_point<ch::system_clock, duration>;
constexpr const char* module_name = "date_time";

template <class Clock>
time_point common_time_point_cast(const ch::time_point<Clock, duration>& tp) {
    return time_point(tp.time_since_epoch());
}
time_point* to_time_point(lua_State* L, int idx);
time_point& new_time_point(lua_State* L, const time_point& v);
duration* to_duration(lua_State* L, int idx);
duration& new_duration(lua_State* L, const duration& v);
void register_duration(lua_State* L);
