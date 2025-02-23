#include "gui.hpp"
#include <span>
using std::span;

static auto window(lua_State* L) -> int {
    auto title = luaL_checkstring(L, 1);
    auto size = span(luaL_checkvector(L, 2), LUA_VECTOR_SIZE);
    const float default_position[3] = {
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        0,
    };
    auto position = span(luaL_optvector(L, 3, default_position), LUA_VECTOR_SIZE);
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window{
        SDL_CreateWindow(title, position[0], position[1], size[0], size[1], SDL_WINDOW_SHOWN),
        SDL_DestroyWindow
    };
    Window_type::make(L, std::move(window));
    return 1;
};

DLUAUSTD_API void dlinit(lua_State* L) {
    SDL_Init(SDL_INIT_VIDEO);
    dluau_addctask([](const char**) -> dluau_CTaskStatus {
        static SDL_Event e;
        bool is_quit{};
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    is_quit = true;
                break;
            }
        }
        if (is_quit) {
            SDL_Quit();
            return DLUAU_CTASK_DONE;
        }
        return DLUAU_CTASK_CONTINUE;
    });
}

DLUAUSTD_API auto dlrequire(lua_State* L) -> int {
    constexpr luaL_Reg lib[] = {
        {"window", window},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    return 1;
}
