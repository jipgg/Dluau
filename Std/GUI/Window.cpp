#include "gui.hpp"
using Registry = Window_type::Registry;
using Init_info = Window_type::Init_info;
using Self = Window_type::Ty_t;

static const Registry namecalls {
    {"destroy", [](lua_State* L, Self& self) -> int {
        self.reset();
        return 0;
    }},
    {"set_borderless", [](lua_State* L, Self& self) -> int {
        SDL_SetWindowBordered(self.get(), static_cast<SDL_bool>(luaL_optboolean(L, 2, false)));
        return 0;
    }},
    {"set_always_on_top", [](lua_State* L, Self& self) -> int {
        SDL_SetWindowAlwaysOnTop(self.get(), static_cast<SDL_bool>(luaL_optboolean(L, 2, true)));
        return 0;
    }},
};

static const Registry index {
    {"size", [](lua_State* L, Self& self) -> int {
        int w, h;
        SDL_GetWindowSize(self.get(), &w, &h);
        lua_pushvector(L, w, h, 0);
        return 1;
    }},
    {"position", [](lua_State* L, Self& self) -> int {
        int x, y;
        SDL_GetWindowPosition(self.get(), &x, &y);
        lua_pushvector(L, x, y, 0);
        return 1;
    }},
    {"opacity", [](lua_State* L, Self& self) -> int {
        float opacity;
        SDL_GetWindowOpacity(self.get(), &opacity);
        dluau::push(L, opacity);
        return 1;
    }},
    {"title", [](lua_State* L, Self& self) -> int {
        dluau::push(L, SDL_GetWindowTitle(self.get()));
        return 1;
    }},
};
static const Registry newindex {
    {"size", [](lua_State* L, Self& self) -> int {
        auto v = dluau::check_vector(L, 3);
        SDL_SetWindowSize(self.get(), v[0], v[1]);
        return 0;
    }},
    {"position", [](lua_State* L, Self& self) -> int {
        auto v = dluau::check_vector(L, 3);
        SDL_SetWindowPosition(self.get(), v[0], v[1]);
        return 0;
    }},
    {"opacity", [](lua_State* L, Self& self) -> int {
        SDL_SetWindowOpacity(self.get(), static_cast<float>((luaL_checknumber(L, 3))));
        return 1;
    }},
    {"title", [](lua_State* L, Self& self) -> int {
        SDL_SetWindowTitle(self.get(), luaL_checkstring(L, 3));
        return 1;
    }},
};

template<> const Init_info Window_type::init_info {
    .index = index,
    .newindex = newindex,
    .namecall = namecalls,
};
