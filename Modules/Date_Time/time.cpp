#include "module.hpp"
#include <lumin.h>
#include <format>
#include <chrono>
#include "userdata_lazybuilder.hpp"
#include <string>
using namespace std::string_literals;
using udata = generic_userdata_template<time_>;
static const std::string tname = module_name + "."s + "time";
template<> const char* udata::type_name(){return tname.c_str();}

time_* to_time(lua_State* L, int idx) {
    return &udata::check_udata(L, idx);
}

static const udata::registry namecall = {
    {"format", [](lua_State* L, time_& tp) -> int {
        const std::string fmt = "{:"s + luaL_checkstring(L, 2) + "}"s;
        lua_pushstring(L, std::vformat(fmt, std::make_format_args(tp)).c_str());
        return 1;
    }},
    {"type", [](lua_State* L, time_& d) -> int {
        lua_pushstring(L, tname.c_str());
        return 1;
    }},
};

static const udata::registry index = {
    {"year", [](lua_State* L, time_& tp) -> int {
        ch::year_month_day ymd{ch::floor<ch::days>(tp)};
        lua_pushinteger(L, static_cast<int>(ymd.year()));
        return 1;
    }},
    {"month", [](lua_State* L, time_& tp) -> int {
        ch::year_month_day ymd{ch::floor<ch::days>(tp)};
        lua_pushinteger(L, static_cast<unsigned int>(ymd.month()));
        return 1;
    }},
    {"day", [](lua_State* L, time_& tp) -> int {
        ch::year_month_day ymd{ch::floor<ch::days>(tp)};
        lua_pushinteger(L, static_cast<unsigned int>(ymd.day()));
        return 1;
    }},
    {"hour", [](lua_State* L, time_& tp) -> int {
        const auto midnight = tp - ch::floor<ch::days>(tp);
        const auto hours = ch::duration_cast<ch::hours>(midnight);
        lua_pushinteger(L, hours.count());
        return 1;
    }},
    {"minute", [](lua_State* L, time_& tp) -> int {
        const auto midnight = tp - ch::floor<ch::days>(tp);
        const auto hours = ch::duration_cast<ch::hours>(midnight);
        const auto minutes = ch::duration_cast<ch::minutes>(midnight - hours);
        lua_pushinteger(L, minutes.count());
        return 1;
    }},
    {"second", [](lua_State* L, time_& tp) -> int {
        const auto midnight = tp - ch::floor<ch::days>(tp);
        const auto hours = ch::duration_cast<ch::hours>(midnight);
        const auto minutes = ch::duration_cast<ch::minutes>(midnight - hours);
        const auto seconds = ch::duration_cast<ch::seconds>(midnight - hours - minutes);
        lua_pushinteger(L, seconds.count());
        return 1;
    }},
};

static int tostring(lua_State* L) {
    time_& tp = udata::check_udata(L, 1);
    lua_pushstring(L, std::format("{:%x %X}", tp).c_str());
    return 1;
}

time_& new_time(lua_State* L, const time_& v) {
    if (not udata::initialized(L)) {
        const luaL_Reg meta[] = {
            {"__tostring", tostring},
            {nullptr, nullptr}
        };
        udata::init(L, {
            .index = index,
            .namecall = namecall,
            .meta = meta,
        });
    }
    return udata::new_udata(L, v);
}
