#pragma once
#include <dluau.hpp>
#include <std.hpp>
#ifdef _WIN32
#include "Windows.h"
#endif

void register_windows_lib(lua_State* L);

