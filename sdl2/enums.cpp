#include "common.hpp"

void register_enums(lua_State *L) {
    auto push_v = [&L](const char* field, int v) {
        lua_pushinteger(L, v);
        lua_setfield(L, -2, field);
    };
    lua_newtable(L);
    push_v("QUIT", SDL_EventType::SDL_QUIT);
    push_v("KEYUP", SDL_EventType::SDL_KEYUP);
    push_v("KEYDOWN", SDL_EventType::SDL_KEYDOWN);
    push_v("DROPFILE", SDL_EventType::SDL_DROPFILE);
    push_v("DROPTEXT", SDL_EventType::SDL_DROPTEXT);
    push_v("FINGERUP", SDL_EventType::SDL_FINGERUP);
    push_v("LASTEVENT", SDL_EventType::SDL_LASTEVENT);
    push_v("USEREVENT", SDL_EventType::SDL_USEREVENT);
    push_v("DROPBEGIN", SDL_EventType::SDL_DROPBEGIN);
    push_v("TEXTINPUT", SDL_EventType::SDL_TEXTINPUT);
    push_v("FIRSTEVENT", SDL_EventType::SDL_FIRSTEVENT);
    push_v("SYSWMEVENT", SDL_EventType::SDL_SYSWMEVENT);
    push_v("FINGERDOWN", SDL_EventType::SDL_FINGERDOWN);
    push_v("MOUSEWHEEL", SDL_EventType::SDL_MOUSEWHEEL);
    push_v("WINDOWEVENT", SDL_EventType::SDL_WINDOWEVENT);
    push_v("JOYBUTTONUP", SDL_EventType::SDL_JOYBUTTONUP);
    push_v("MOUSEMOTION", SDL_EventType::SDL_MOUSEMOTION);
    push_v("TEXTEDITING", SDL_EventType::SDL_TEXTEDITING);
    push_v("DISPLAYEVENT", SDL_EventType::SDL_DISPLAYEVENT);
    push_v("MOUSEBUTTONUP", SDL_EventType::SDL_MOUSEBUTTONUP);
    push_v("MOUSEBUTTONDOWN", SDL_EventType::SDL_MOUSEBUTTONDOWN);
    lua_setfield(L, -2, "EventType");

    lua_newtable(L);
    push_v("SHOWN", SDL_WindowFlags::SDL_WINDOW_SHOWN);
    push_v("RESIZABLE", SDL_WindowFlags::SDL_WINDOW_RESIZABLE);
    push_v("METAL", SDL_WindowFlags::SDL_WINDOW_METAL);
    push_v("HIDDEN", SDL_WindowFlags::SDL_WINDOW_HIDDEN);
    push_v("OPENGL", SDL_WindowFlags::SDL_WINDOW_OPENGL);
    push_v("VULKAN", SDL_WindowFlags::SDL_WINDOW_VULKAN);
    push_v("FOREIGN", SDL_WindowFlags::SDL_WINDOW_FOREIGN);
    push_v("TOOLTIP", SDL_WindowFlags::SDL_WINDOW_TOOLTIP);
    push_v("UTILITY", SDL_WindowFlags::SDL_WINDOW_UTILITY);
    push_v("MAXIMIZED", SDL_WindowFlags::SDL_WINDOW_MAXIMIZED);
    push_v("MINIMIZED", SDL_WindowFlags::SDL_WINDOW_MINIMIZED);
    push_v("BORDERLESS", SDL_WindowFlags::SDL_WINDOW_BORDERLESS);
    push_v("FULLSCREEN", SDL_WindowFlags::SDL_WINDOW_FULLSCREEN);
    push_v("MENU", SDL_WindowFlags::SDL_WINDOW_POPUP_MENU);
    push_v("INPUT_FOCUS", SDL_WindowFlags::SDL_WINDOW_INPUT_FOCUS);
    push_v("MOUSE_FOCUS", SDL_WindowFlags::SDL_WINDOW_MOUSE_FOCUS);
    push_v("SKIP_TASKBAR", SDL_WindowFlags::SDL_WINDOW_SKIP_TASKBAR);
    push_v("MOUSE_CAPTURE", SDL_WindowFlags::SDL_WINDOW_MOUSE_CAPTURE);
    push_v("MOUSE_GRABBED", SDL_WindowFlags::SDL_WINDOW_MOUSE_GRABBED);
    push_v("INPUT_GRABBED", SDL_WindowFlags::SDL_WINDOW_INPUT_GRABBED);
    push_v("ALLOW_HIGHDPI", SDL_WindowFlags::SDL_WINDOW_ALLOW_HIGHDPI);
    push_v("ALWAYS_ON_TOP", SDL_WindowFlags::SDL_WINDOW_ALWAYS_ON_TOP);
    push_v("KEYBOARD_GRABBED", SDL_WindowFlags::SDL_WINDOW_KEYBOARD_GRABBED);
    push_v("FULLSCREEN_DESKTOP", SDL_WindowFlags::SDL_WINDOW_FULLSCREEN_DESKTOP);
    lua_setfield(L, -2, "WindowFlags");

    lua_newtable(L);
    push_v("SOFTWARE", SDL_RendererFlags::SDL_RENDERER_SOFTWARE);
    push_v("ACCELERATED", SDL_RendererFlags::SDL_RENDERER_ACCELERATED);
    push_v("PRESENTVSYNC", SDL_RendererFlags::SDL_RENDERER_PRESENTVSYNC);
    push_v("TARGETTEXTURE", SDL_RendererFlags::SDL_RENDERER_TARGETTEXTURE);
    lua_setfield(L, -2, "RendererFlags");
}
