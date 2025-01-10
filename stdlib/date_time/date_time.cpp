#include "_luminstd_api.h"
#include "date_time.hpp"

static int utc_now(lua_State* L) {
    newdatetime(L, ch::system_clock::now());
    return 1;
}
static int local_now(lua_State* L) {
    using sc = ch::system_clock;
    auto local_zone = ch::current_zone();
    const auto local_time = ch::zoned_time<sc::duration>(
        ch::current_zone(), sc::now()
    ).get_local_time();
    newdatetime(L, sc::time_point(local_time.time_since_epoch()));
    return 1;
}

LUMINSTD_API inline int luminstd_datetime(lua_State* L) {
    const luaL_Reg lib[] = {
        {"utc_now", utc_now},
        {"local_now", local_now},
        {"seconds", [](lua_State* L) -> int {
            newduration(L, ch::seconds{luaL_checkinteger(L, 1)});
            return 1;
        }},
        {"nanoseconds", [](lua_State* L) -> int {
            newduration(L, ch::nanoseconds{luaL_checkinteger(L, 1)});
            return 1;
        }},
        {"microseconds", [](lua_State* L) -> int {
            newduration(L, ch::microseconds{luaL_checkinteger(L, 1)});
            return 1;
        }},
        {"minutes", [](lua_State* L) -> int {
            newduration(L, ch::minutes{luaL_checkinteger(L, 1)});
            return 1;
        }},
        {"hours", [](lua_State* L) -> int {
            newduration(L, ch::hours{luaL_checkinteger(L, 1)});
            return 1;
        }},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    return 1;
}
