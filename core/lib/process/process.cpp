#include <lumin.h>
#include <core.hpp>
#include <lualib.h>
#include <ranges>


void luminopen_process(lua_State *L) {
    lua_newtable(L);
    lua_newtable(L);
    int i{1};
    for (auto sr : std::views::split(process_args, ' ')) {
        lua_pushlstring(L, sr.data(), sr.size());
        lua_rawseti(L, -2, i++);
    }
    lua_setreadonly(L, -1, true);
    lua_setfield(L, -2, "args");
    lua_setglobal(L, "process");
}
