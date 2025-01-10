#include "_luminstd_api.h"
#include "date_time.hpp"

static int utc_now(lua_State* L) {
    new_time_point(L, ch::system_clock::now());
    return 1;
}
static int time_now(lua_State* L) {
    using sc = ch::system_clock;
    auto local_zone = ch::current_zone();
    const auto local_time = ch::zoned_time<sc::duration>(
        local_zone, sc::now()
    ).get_local_time();
    new_time_point(L, sc::time_point(local_time.time_since_epoch()));
    return 1;
}
static int zoned_time(lua_State* L) {
    using ch::zoned_time;
    time_point& tp = *to_time_point(L, 1);
    return 1;
}

template<class Clock>
static void register_clock_variant(lua_State* L, const char* as) {
    const luaL_Reg lib[] = {
        {"now", [](lua_State* L) -> int {
            new_time_point(L, common_time_point_cast<Clock>(Clock::now()));
            return 1;
        }},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    lua_setfield(L, -2, as);
}
template<class Duration>
static int new_ratio(lua_State* L) {
    new_duration(L, Duration{luaL_checkinteger(L, 1)});
    return 1;
}

LUMINSTD_API inline int module_date_time(lua_State* L) {
    const luaL_Reg lib[] = {
        {"time_now", time_now},
        {"seconds", new_ratio<ch::seconds>},
        {"nanoseconds", new_ratio<ch::nanoseconds>},
        {"microseconds", new_ratio<ch::microseconds>},
        {"minutes", new_ratio<ch::minutes>},
        {"hours", new_ratio<ch::hours>},
        {"days", new_ratio<ch::days>},
        {"months", new_ratio<ch::months>},
        {"years", new_ratio<ch::years>},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    register_clock_variant<ch::system_clock>(L, "system_clock");
    register_clock_variant<ch::steady_clock>(L, "steady_clock");
    register_clock_variant<ch::utc_clock>(L, "utc_clock");
    register_clock_variant<ch::high_resolution_clock>(L, "high_resolution_clock");
    register_clock_variant<ch::gps_clock>(L, "gps_clock");
    register_clock_variant<ch::tai_clock>(L, "tai_clock");
    return 1;
}
