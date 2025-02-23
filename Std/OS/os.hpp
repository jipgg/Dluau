#pragma once
#include <dluau.hpp>
#include <std.hpp>
#ifdef _WIN32
#include "Windows.h"
#endif

namespace os {
void register_windows_api(lua_State* L);
auto keydown(lua_State* L) -> int;
auto messagebox(lua_State* L) -> int;
}
