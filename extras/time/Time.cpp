#include "module.hpp"
#include <lumin.h>
#include <format>
#include <chrono>
#include "Userdata_template.hpp"
#include <string>
using namespace std::string_literals;
using Time_ut = Userdata_template<Time>;
static const std::string tname = module_name + "."s + "Time";
template<> const char* Time_ut::type_name(){return tname.c_str();}

Time* to_time(lua_State* L, int idx) {
    return &Time_ut::check_udata(L, idx);
}

static const Time_ut::Registry namecall = {
    {"format", [](lua_State* L, Time& tp) -> int {
        const std::string fmt = "{:"s + luaL_checkstring(L, 2) + "}"s;
        lua_pushstring(L, std::vformat(fmt, std::make_format_args(tp)).c_str());
        return 1;
    }},
    {"type", [](lua_State* L, Time& d) -> int {
        lua_pushstring(L, tname.c_str());
        return 1;
    }},
};

static const Time_ut::Registry index = {
    {"year", [](lua_State* L, Time& tp) -> int {
        ch::year_month_day ymd{ch::floor<ch::days>(tp)};
        lua_pushinteger(L, static_cast<int>(ymd.year()));
        return 1;
    }},
    {"month", [](lua_State* L, Time& tp) -> int {
        ch::year_month_day ymd{ch::floor<ch::days>(tp)};
        lua_pushinteger(L, static_cast<unsigned int>(ymd.month()));
        return 1;
    }},
    {"day", [](lua_State* L, Time& tp) -> int {
        ch::year_month_day ymd{ch::floor<ch::days>(tp)};
        lua_pushinteger(L, static_cast<unsigned int>(ymd.day()));
        return 1;
    }},
    {"hour", [](lua_State* L, Time& tp) -> int {
        const auto midnight = tp - ch::floor<ch::days>(tp);
        const auto hours = ch::duration_cast<ch::hours>(midnight);
        lua_pushinteger(L, hours.count());
        return 1;
    }},
    {"minute", [](lua_State* L, Time& tp) -> int {
        const auto midnight = tp - ch::floor<ch::days>(tp);
        const auto hours = ch::duration_cast<ch::hours>(midnight);
        const auto minutes = ch::duration_cast<ch::minutes>(midnight - hours);
        lua_pushinteger(L, minutes.count());
        return 1;
    }},
    {"second", [](lua_State* L, Time& tp) -> int {
        const auto midnight = tp - ch::floor<ch::days>(tp);
        const auto hours = ch::duration_cast<ch::hours>(midnight);
        const auto minutes = ch::duration_cast<ch::minutes>(midnight - hours);
        const auto seconds = ch::duration_cast<ch::seconds>(midnight - hours - minutes);
        lua_pushinteger(L, seconds.count());
        return 1;
    }},
};

static int tostring(lua_State* L) {
    Time& tp = Time_ut::check_udata(L, 1);
    lua_pushstring(L, std::format("{:%x %X}", tp).c_str());
    return 1;
}

Time& new_time(lua_State* L, const Time& v) {
    if (not Time_ut::initialized(L)) {
        const luaL_Reg meta[] = {
            {"__tostring", tostring},
            {nullptr, nullptr}
        };
        Time_ut::init(L, {
            .index = index,
            .namecall = namecall,
            .meta = meta,
        });
    }
    return Time_ut::new_udata(L, v);
}
