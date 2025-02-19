#include "time.hpp"
using std::optional, std::nullopt;
using namespace std::chrono;

static optional<int> opt_int_field(lua_State* L, int idx, const std::string& name) {
    lua_getfield(L, idx, name.c_str());
    if (lua_isnumber(L, -1)) {
        const int v = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        return v;
    }
    return nullopt;
}
static int time_now(lua_State* L) {
    const time_zone* zone = current_zone();
    if (lua_isstring(L, 1)) zone = locate_zone(lua_tostring(L, 1));
    auto now = zoned_time(zone, time_point_cast<milliseconds>(system_clock::now()));
    T_time_point::make(L, now);
    return 1;
}
static int utc_now(lua_State* L) {
    auto now = zoned_time(time_point_cast<milliseconds>(system_clock::now()));
    T_time_point::make(L, now);
    return 1;
}
static int nano_now(lua_State* L) {
    T_nano_time_point::make(L, Nano_time_point(duration_cast<nanoseconds>(steady_clock::now().time_since_epoch())));
    return 1;
}
static int from_datetime(lua_State* L) {
    year_month_day ymd =
        year{luaL_checkinteger(L, 1)}
        / month{static_cast<unsigned>(luaL_checkinteger(L, 2))}
        / day{static_cast<unsigned>(luaL_checkinteger(L, 3))};
    hours h{luaL_checkinteger(L, 4)};
    minutes m{luaL_checkinteger(L, 5)};
    seconds s{luaL_checkinteger(L, 6)};
    T_time_point::make(L, Time_point(current_zone(), local_days(ymd) + h + m + s));
    return 1;
}
static int from_date(lua_State* L) {
    year_month_day ymd =
        year{luaL_checkinteger(L, 1)}
        / month{static_cast<unsigned>(luaL_checkinteger(L, 2))}
        / day{static_cast<unsigned>(luaL_checkinteger(L, 3))};
    T_time_point::make(L, Time_point(current_zone(), local_days(ymd)));
    return 1;
}

static int from_duration(lua_State* L) {
    hours h{luaL_checkinteger(L, 1)};
    minutes m{luaL_checkinteger(L, 2)};
    seconds s{luaL_checkinteger(L, 3)};
    milliseconds ms{luaL_optinteger(L, 4, 0)};
    T_time_span::create(L, h + m + s + ms);
    return 1;
}
static int from_time(lua_State* L) {
    hours h{luaL_checkinteger(L, 1)};
    minutes m{luaL_checkinteger(L, 2)};
    seconds s{luaL_checkinteger(L, 3)};
    milliseconds ms{luaL_optinteger(L, 4, 0)};
    T_time_span::create(L, h + m + s + ms);
    return 1;
}

template<class Duration>
static int new_ratio(lua_State* L) {
    T_time_span::create(L, Duration{luaL_checkinteger(L, 1)});
    return 1;
}

DLUAUSTD_API inline int dlrequire(lua_State* L) {
    const luaL_Reg lib[] = {
        {"now", time_now},
        {"utc_now", utc_now},
        {"nano_now", nano_now},
        {"from_datetime" , from_datetime},
        {"from_date" , from_date},
        {"from_time" , from_time},
        {"seconds", new_ratio<seconds>},
        {"nanoseconds", new_ratio<nanoseconds>},
        {"microseconds", new_ratio<microseconds>},
        {"minutes", new_ratio<minutes>},
        {"hours", new_ratio<hours>},
        {"days", new_ratio<days>},
        {"months", new_ratio<months>},
        {"years", new_ratio<years>},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    auto zone = current_zone()->name();
    lua_pushlstring(L, zone.data(), zone.length());
    lua_setfield(L, -2, "current_zone");
    lua_setreadonly(L, -1, true);
    return 1;
}
