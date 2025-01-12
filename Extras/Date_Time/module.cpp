#include ".lumin_extras_api.h"
#include "module.hpp"
using ch::time_point_cast;
static std::optional<int> opt_int_field(lua_State* L, int idx, const std::string& name) {
    lua_getfield(L, idx, name.c_str());
    if (lua_isnumber(L, -1)) {
        const int v = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        return v;
    }
    return std::nullopt;
}
static int time_now(lua_State* L) {
    using sys = ch::system_clock;
    auto local_zone = ch::current_zone();
    auto now = ch::time_point_cast<ch::milliseconds>(sys::now());
    using zoned = ch::zoned_time<ch::milliseconds>;
    const auto local_time = zoned {local_zone, now};
    new_time(L, local_time);
    return 1;
}
static int from_datetime(lua_State* L) {
    ch::year_month_day ymd =
        ch::year{luaL_checkinteger(L, 1)}
        / ch::month{static_cast<unsigned>(luaL_checkinteger(L, 2))}
        / ch::day{static_cast<unsigned>(luaL_checkinteger(L, 3))};
    ch::hours h{luaL_checkinteger(L, 4)};
    ch::minutes m{luaL_checkinteger(L, 5)};
    ch::seconds s{luaL_checkinteger(L, 6)};
    new_time(L, ch::sys_days(ymd) + h + m + s);
    return 1;
}
static int from_date(lua_State* L) {
    ch::year_month_day ymd =
        ch::year{luaL_checkinteger(L, 1)}
        / ch::month{static_cast<unsigned>(luaL_checkinteger(L, 2))}
        / ch::day{static_cast<unsigned>(luaL_checkinteger(L, 3))};
    new_time(L, ch::sys_days(ymd));
    return 1;
}

static int from_duration(lua_State* L) {
    ch::hours h{luaL_checkinteger(L, 1)};
    ch::minutes m{luaL_checkinteger(L, 2)};
    ch::seconds s{luaL_checkinteger(L, 3)};
    ch::milliseconds ms{luaL_optinteger(L, 4, 0)};
    new_duration(L, h + m + s + ms);
    return 1;
}
static int from_time(lua_State* L) {
    ch::hours h{luaL_checkinteger(L, 1)};
    ch::minutes m{luaL_checkinteger(L, 2)};
    ch::seconds s{luaL_checkinteger(L, 3)};
    ch::milliseconds ms{luaL_optinteger(L, 4, 0)};
    new_duration(L, h + m + s + ms);
    return 1;
}

template<class Clock>
static void register_clock_variant(lua_State* L, const char* as) {
    const luaL_Reg lib[] = {
        {"now", [](lua_State* L) -> int {
            new_time(L, time_point_cast<ch::milliseconds>(Clock::now()));
            return 1;
        }},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    lua_setfield(L, -2, as);
}
template<class Clock>
static void register_precise_clock_variant(lua_State* L, const char* as) {
    const luaL_Reg lib[] = {
        {"now", [](lua_State* L) -> int {
            new_high_precision_time(L, high_precision_time(ch::duration_cast<ch::nanoseconds>(Clock::now().time_since_epoch())));
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

LUMIN_EXTRAS_API inline int loadmodule(lua_State* L) {
    const luaL_Reg lib[] = {
        {"now", time_now},
        {"datetime" , from_datetime},
        {"date" , from_date},
        {"time" , from_time},
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
    register_precise_clock_variant<ch::steady_clock>(L, "high_precision_clock");
    //register_clock_variant<ch::utc_clock>(L, "utc_clock");
    //register_precise_clock_variant<ch::high_resolution_clock>(L, "high_resolution_clock");
    //register_precise_clock_variant<ch::gps_clock>(L, "gps_clock");
    //register_precise_clock_variant<ch::tai_clock>(L, "tai_clock");
    return 1;
}
