#pragma once
#include <dluau.hpp>
#include <std.hpp>
#include <SDL.h>
#include <Lazy_type.hpp>

struct Window_type_info {
    static consteval const char* type_namespace() {return "std.gui";}
    static consteval const char* type_name() {return "window";}
};

using Window_type = Lazy_type<std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>, Window_type_info>;
