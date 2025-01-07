#include "common.hpp"
#include <lualib.h>
#include "halua/libapi.h"
static const int tag = halua_newtypetag();

static int index(lua_State* L) {
    SDL_Event& self = *static_cast<SDL_Event*>(lua_touserdatatagged(L, 1, tag));
    std::string_view key = luaL_checkstring(L, 2);
    if (key == "type") {
        lua_pushinteger(L, self.type);
        return 1;
    }
    luaL_argerrorL(L, 2, "invalid index");
}

int event_tag() {
    return tag;
}

void event_init(lua_State* L) {
    const luaL_Reg meta[] = {
        {"__index", index},
        {nullptr, nullptr}
    };
    if (luaL_newmetatable(L, event_type)) {
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, event_type);
        lua_setfield(L, -2, "__type");
        lua_setuserdatadtor(L, tag, [](lua_State* L, void* ud) {
            static_cast<SDL_Event*>(ud)->~SDL_Event();
        });
    }
    lua_pop(L, 1);
}
int event_ctor_new(lua_State* L) {
    SDL_Event* ud = static_cast<SDL_Event*>(lua_newuserdatatagged(L, sizeof(SDL_Event), tag));
    luaL_getmetatable(L, event_type);
    lua_setmetatable(L, -2);
    new (ud) SDL_Event{};
    return 1;
}
int event_ctor(lua_State *L) {
    lua_newtable(L);
    lua_pushcfunction(L, event_ctor_new, "SDL2.Event.new");
    lua_setfield(L, -2, "new");
    return 1;
}
