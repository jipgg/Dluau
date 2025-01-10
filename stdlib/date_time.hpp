#pragma once
#include <chrono>
#include <lumin.h>
#include <lualib.h>
namespace ch = std::chrono;
using datetime = ch::time_point<ch::system_clock>;
using duration = ch::nanoseconds;

datetime* todatetime(lua_State* L, int idx);
datetime& newdatetime(lua_State* L, const datetime& v);
duration* toduration(lua_State* L, int idx);
duration& newduration(lua_State* L, const duration& v);
void register_duration(lua_State* L);
