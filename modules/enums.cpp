#include "common.hpp"
auto push_int_field = [](lua_State* L, const char* field, int v) {
        lua_pushinteger(L, v);
        lua_setfield(L, -2, field);
};

void push_window_flags(lua_State* L) {
    lua_newtable(L);
    push_int_field(L, "SHOWN", SDL_WindowFlags::SDL_WINDOW_SHOWN);
    push_int_field(L, "RESIZABLE", SDL_WindowFlags::SDL_WINDOW_RESIZABLE);
    push_int_field(L, "METAL", SDL_WindowFlags::SDL_WINDOW_METAL);
    push_int_field(L, "HIDDEN", SDL_WindowFlags::SDL_WINDOW_HIDDEN);
    push_int_field(L, "OPENGL", SDL_WindowFlags::SDL_WINDOW_OPENGL);
    push_int_field(L, "VULKAN", SDL_WindowFlags::SDL_WINDOW_VULKAN);
    push_int_field(L, "FOREIGN", SDL_WindowFlags::SDL_WINDOW_FOREIGN);
    push_int_field(L, "TOOLTIP", SDL_WindowFlags::SDL_WINDOW_TOOLTIP);
    push_int_field(L, "UTILITY", SDL_WindowFlags::SDL_WINDOW_UTILITY);
    push_int_field(L, "MAXIMIZED", SDL_WindowFlags::SDL_WINDOW_MAXIMIZED);
    push_int_field(L, "MINIMIZED", SDL_WindowFlags::SDL_WINDOW_MINIMIZED);
    push_int_field(L, "BORDERLESS", SDL_WindowFlags::SDL_WINDOW_BORDERLESS);
    push_int_field(L, "FULLSCREEN", SDL_WindowFlags::SDL_WINDOW_FULLSCREEN);
    push_int_field(L, "MENU", SDL_WindowFlags::SDL_WINDOW_POPUP_MENU);
    push_int_field(L, "INPUT_FOCUS", SDL_WindowFlags::SDL_WINDOW_INPUT_FOCUS);
    push_int_field(L, "MOUSE_FOCUS", SDL_WindowFlags::SDL_WINDOW_MOUSE_FOCUS);
    push_int_field(L, "SKIP_TASKBAR", SDL_WindowFlags::SDL_WINDOW_SKIP_TASKBAR);
    push_int_field(L, "MOUSE_CAPTURE", SDL_WindowFlags::SDL_WINDOW_MOUSE_CAPTURE);
    push_int_field(L, "MOUSE_GRABBED", SDL_WindowFlags::SDL_WINDOW_MOUSE_GRABBED);
    push_int_field(L, "INPUT_GRABBED", SDL_WindowFlags::SDL_WINDOW_INPUT_GRABBED);
    push_int_field(L, "ALLOW_HIGHDPI", SDL_WindowFlags::SDL_WINDOW_ALLOW_HIGHDPI);
    push_int_field(L, "ALWAYS_ON_TOP", SDL_WindowFlags::SDL_WINDOW_ALWAYS_ON_TOP);
    push_int_field(L, "KEYBOARD_GRABBED", SDL_WindowFlags::SDL_WINDOW_KEYBOARD_GRABBED);
    push_int_field(L, "FULLSCREEN_DESKTOP", SDL_WindowFlags::SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void register_enums(lua_State *L) {
    auto push_int_field = [&L](const char* field, int v) {
        lua_pushinteger(L, v);
        lua_setfield(L, -2, field);
    };
    lua_newtable(L);
    push_int_field("QUIT", SDL_EventType::SDL_QUIT);
    push_int_field("KEYUP", SDL_EventType::SDL_KEYUP);
    push_int_field("KEYDOWN", SDL_EventType::SDL_KEYDOWN);
    push_int_field("DROPFILE", SDL_EventType::SDL_DROPFILE);
    push_int_field("DROPTEXT", SDL_EventType::SDL_DROPTEXT);
    push_int_field("FINGERUP", SDL_EventType::SDL_FINGERUP);
    push_int_field("LASTEVENT", SDL_EventType::SDL_LASTEVENT);
    push_int_field("USEREVENT", SDL_EventType::SDL_USEREVENT);
    push_int_field("DROPBEGIN", SDL_EventType::SDL_DROPBEGIN);
    push_int_field("TEXTINPUT", SDL_EventType::SDL_TEXTINPUT);
    push_int_field("FIRSTEVENT", SDL_EventType::SDL_FIRSTEVENT);
    push_int_field("SYSWMEVENT", SDL_EventType::SDL_SYSWMEVENT);
    push_int_field("FINGERDOWN", SDL_EventType::SDL_FINGERDOWN);
    push_int_field("MOUSEWHEEL", SDL_EventType::SDL_MOUSEWHEEL);
    push_int_field("WINDOWEVENT", SDL_EventType::SDL_WINDOWEVENT);
    push_int_field("JOYBUTTONUP", SDL_EventType::SDL_JOYBUTTONUP);
    push_int_field("MOUSEMOTION", SDL_EventType::SDL_MOUSEMOTION);
    push_int_field("TEXTEDITING", SDL_EventType::SDL_TEXTEDITING);
    push_int_field("DISPLAYEVENT", SDL_EventType::SDL_DISPLAYEVENT);
    push_int_field("MOUSEBUTTONUP", SDL_EventType::SDL_MOUSEBUTTONUP);
    push_int_field("MOUSEBUTTONDOWN", SDL_EventType::SDL_MOUSEBUTTONDOWN);
    lua_setfield(L, -2, "EVENT_TYPE");

    lua_newtable(L);
    push_int_field("SHOWN", SDL_WindowFlags::SDL_WINDOW_SHOWN);
    push_int_field("RESIZABLE", SDL_WindowFlags::SDL_WINDOW_RESIZABLE);
    push_int_field("METAL", SDL_WindowFlags::SDL_WINDOW_METAL);
    push_int_field("HIDDEN", SDL_WindowFlags::SDL_WINDOW_HIDDEN);
    push_int_field("OPENGL", SDL_WindowFlags::SDL_WINDOW_OPENGL);
    push_int_field("VULKAN", SDL_WindowFlags::SDL_WINDOW_VULKAN);
    push_int_field("FOREIGN", SDL_WindowFlags::SDL_WINDOW_FOREIGN);
    push_int_field("TOOLTIP", SDL_WindowFlags::SDL_WINDOW_TOOLTIP);
    push_int_field("UTILITY", SDL_WindowFlags::SDL_WINDOW_UTILITY);
    push_int_field("MAXIMIZED", SDL_WindowFlags::SDL_WINDOW_MAXIMIZED);
    push_int_field("MINIMIZED", SDL_WindowFlags::SDL_WINDOW_MINIMIZED);
    push_int_field("BORDERLESS", SDL_WindowFlags::SDL_WINDOW_BORDERLESS);
    push_int_field("FULLSCREEN", SDL_WindowFlags::SDL_WINDOW_FULLSCREEN);
    push_int_field("MENU", SDL_WindowFlags::SDL_WINDOW_POPUP_MENU);
    push_int_field("INPUT_FOCUS", SDL_WindowFlags::SDL_WINDOW_INPUT_FOCUS);
    push_int_field("MOUSE_FOCUS", SDL_WindowFlags::SDL_WINDOW_MOUSE_FOCUS);
    push_int_field("SKIP_TASKBAR", SDL_WindowFlags::SDL_WINDOW_SKIP_TASKBAR);
    push_int_field("MOUSE_CAPTURE", SDL_WindowFlags::SDL_WINDOW_MOUSE_CAPTURE);
    push_int_field("MOUSE_GRABBED", SDL_WindowFlags::SDL_WINDOW_MOUSE_GRABBED);
    push_int_field("INPUT_GRABBED", SDL_WindowFlags::SDL_WINDOW_INPUT_GRABBED);
    push_int_field("ALLOW_HIGHDPI", SDL_WindowFlags::SDL_WINDOW_ALLOW_HIGHDPI);
    push_int_field("ALWAYS_ON_TOP", SDL_WindowFlags::SDL_WINDOW_ALWAYS_ON_TOP);
    push_int_field("KEYBOARD_GRABBED", SDL_WindowFlags::SDL_WINDOW_KEYBOARD_GRABBED);
    push_int_field("FULLSCREEN_DESKTOP", SDL_WindowFlags::SDL_WINDOW_FULLSCREEN_DESKTOP);
    lua_setfield(L, -2, "WINDOW_FLAGS");

    lua_newtable(L);
    push_int_field("SOFTWARE", SDL_RendererFlags::SDL_RENDERER_SOFTWARE);
    push_int_field("ACCELERATED", SDL_RendererFlags::SDL_RENDERER_ACCELERATED);
    push_int_field("PRESENTVSYNC", SDL_RendererFlags::SDL_RENDERER_PRESENTVSYNC);
    push_int_field("TARGETTEXTURE", SDL_RendererFlags::SDL_RENDERER_TARGETTEXTURE);
    lua_setfield(L, -2, "RENDERER_FLAGS");
}
