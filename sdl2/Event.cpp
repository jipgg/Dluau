#include "common.hpp"
#include <lualib.h>
#include "halua/libapi.h"
static const int tag = halua_newtypetag();
static const char* tname = "SDL_Event";

static int index(lua_State* L) {
    SDL_Event& self = *static_cast<SDL_Event*>(lua_touserdatatagged(L, 1, tag));
    std::string_view key = luaL_checkstring(L, 2);
    if (key == "type") {
        lua_pushinteger(L, self.type);
        return 1;
    }
    luaL_argerrorL(L, 2, "invalid index");
}

static void init(lua_State* L) {
    const luaL_Reg meta[] = {
        {"__index", index},
        {nullptr, nullptr}
    };
    if (luaL_newmetatable(L, tname)) {
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, tname);
        lua_setfield(L, -2, "__type");
        lua_setuserdatadtor(L, tag, [](lua_State* L, void* ud) {
            static_cast<SDL_Event*>(ud)->~SDL_Event();
        });
    }
    lua_pop(L, 1);
}
int event_ctor_new(lua_State* L) {
    SDL_Event* ud = static_cast<SDL_Event*>(lua_newuserdatatagged(L, sizeof(SDL_Event), tag));
    luaL_getmetatable(L, tname);
    lua_setmetatable(L, -2);
    new (ud) SDL_Event{};
    return 1;
}
void register_event(lua_State *L) {
    init(L);
    lua_pushcfunction(L, event_ctor_new, "SDL2.Event");
    lua_setfield(L, -2, "Event");
}
SDL_Event* toevent(lua_State* L, int idx) {
    return static_cast<SDL_Event*>(lua_touserdatatagged(L, idx, tag));
}
