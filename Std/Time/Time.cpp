#include "time.hpp"
#include <format>
#include <chrono>
#include <string>
using namespace std::string_literals;
namespace sc = std::chrono;
using std::string;

static const TimeType::Registry namecall = {
    {"format", [](lua_State* L, Time& tp) -> int {
        const string fmt = "{:"s + luaL_checkstring(L, 2) + "}"s;
        try {
            const auto formatted = std::vformat(fmt, std::make_format_args(tp)); 
            lua_pushstring(L, formatted.c_str());
            return 1;
        } catch (std::format_error& err) {
            luaL_errorL(L, err.what());
        }
    }},
    {"change_zone", [](lua_State* L, Time& tp) -> int {
        const std::string zone = luaL_checkstring(L, 2);
        try {
            sc::zoned_time zoned(zone, tp);
            TimeType::make(L, std::move(zoned));
            return 1;
        } catch (std::runtime_error& err) {
            luaL_errorL(L, format("{} '{}'", err.what(), zone).c_str());
        }
    }},
};

static const TimeType::Registry index = {
    {"year", [](lua_State* L, Time& tp) -> int {
        sc::year_month_day ymd{floor<sc::days>(tp.get_local_time())};
        lua_pushinteger(L, static_cast<int>(ymd.year()));
        return 1;
    }},
    {"month", [](lua_State* L, Time& tp) -> int {
        sc::year_month_day ymd{floor<sc::days>(tp.get_local_time())};
        lua_pushinteger(L, static_cast<unsigned int>(ymd.month()));
        return 1;
    }},
    {"day", [](lua_State* L, Time& tp) -> int {
        sc::year_month_day ymd{floor<sc::days>(tp.get_local_time())};
        lua_pushinteger(L, static_cast<unsigned int>(ymd.day()));
        return 1;
    }},
    {"hour", [](lua_State* L, Time& tp) -> int {
        const auto lt = tp.get_local_time();
        const auto midnight = lt - floor<sc::days>(lt);
        const auto hrs = duration_cast<sc::hours>(midnight);
        lua_pushinteger(L, hrs.count());
        return 1;
    }},
    {"minute", [](lua_State* L, Time& tp) -> int {
        const auto lt = tp.get_local_time();
        const auto midnight = lt - floor<sc::days>(lt);
        const auto h = duration_cast<sc::hours>(midnight);
        const auto min = duration_cast<sc::minutes>(midnight - h);
        lua_pushinteger(L, min.count());
        return 1;
    }},
    {"second", [](lua_State* L, Time& tp) -> int {
        const auto lt = tp.get_local_time();
        const auto midnight = lt - floor<sc::days>(lt);
        const auto hrs = duration_cast<sc::hours>(midnight);
        const auto min = duration_cast<sc::minutes>(midnight - hrs);
        const auto sec = duration_cast<sc::seconds>(midnight - hrs - min);
        lua_pushinteger(L, sec.count());
        return 1;
    }},
    {"millisecond", [](lua_State* L, Time& tp) -> int {
        const auto lt = tp.get_local_time();
        const auto midnight = lt - floor<sc::days>(lt);
        const auto hrs = duration_cast<sc::hours>(midnight);
        const auto min = duration_cast<sc::minutes>(midnight - hrs);
        const auto sec = duration_cast<sc::seconds>(midnight - hrs - min);
        const auto milli = duration_cast<sc::milliseconds>(midnight - hrs - min - sec);
        lua_pushinteger(L, milli.count());
        return 1;
    }},
    {"time_zone", [](lua_State* L, Time& tp) -> int {
        auto timezone = tp.get_time_zone()->name();
        lua_pushlstring(L, timezone.data(), timezone.length());
        return 1;
    }},
    {"zone_abbreviation", [](lua_State* L, Time& tp) -> int {
        auto abbrev = tp.get_info().abbrev;
        lua_pushlstring(L, abbrev.data(), abbrev.length());
        return 1;
    }},
};

static int sub(lua_State* L) {
    TimeSpanType::create(L, TimeType::check(L, 1).get_sys_time() - TimeType::check(L, 2).get_sys_time());
    return 1;
}

static int tostring(lua_State* L) {
    Time& tp = TimeType::check(L, 1);
    lua_pushstring(L, std::format("{}", tp).c_str());
    return 1;
}

constexpr luaL_Reg meta[] = {
    {"__tostring", tostring},
    {"__sub", sub},
    {nullptr, nullptr}
};
template<> const TimeType::InitInfo TimeType::init_info {
    .index = index,
    .namecall = namecall,
    .meta = meta,
};
