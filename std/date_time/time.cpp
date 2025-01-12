#include "date_time.hpp"
#include <lumin.h>
#include <format>
#include <chrono>
#include "userdata_lazybuilder.hpp"
#include <string>
using namespace std::string_literals;
using lb = userdata_lazybuilder<time_type>;
static const std::string tname = module_name + "."s + "time";
template<> const char* lb::type_name(){return tname.c_str();}

time_type* to_time(lua_State* L, int idx) {
    return &lb::check_udata(L, idx);
}

static const lb::registry namecall = {
    {"format", [](lua_State* L, time_type& tp) -> int {
        const std::string fmt = "{:"s + luaL_checkstring(L, 2) + "}"s;
        lua_pushstring(L, std::vformat(fmt, std::make_format_args(tp)).c_str());
        return 1;
    }}
};

static const lb::registry index = {
    {"year", [](lua_State* L, time_type& tp) -> int {
        ch::year_month_day ymd{ch::floor<ch::days>(tp)};
        lua_pushinteger(L, static_cast<int>(ymd.year()));
        return 1;
    }},
    {"month", [](lua_State* L, time_type& tp) -> int {
        ch::year_month_day ymd{ch::floor<ch::days>(tp)};
        lua_pushinteger(L, static_cast<unsigned int>(ymd.month()));
        return 1;
    }},
    {"day", [](lua_State* L, time_type& tp) -> int {
        ch::year_month_day ymd{ch::floor<ch::days>(tp)};
        lua_pushinteger(L, static_cast<unsigned int>(ymd.day()));
        return 1;
    }},
    {"hour", [](lua_State* L, time_type& tp) -> int {
        const auto midnight = tp - ch::floor<ch::days>(tp);
        const auto hours = ch::duration_cast<ch::hours>(midnight);
        lua_pushinteger(L, hours.count());
        return 1;
    }},
    {"minute", [](lua_State* L, time_type& tp) -> int {
        const auto midnight = tp - ch::floor<ch::days>(tp);
        const auto hours = ch::duration_cast<ch::hours>(midnight);
        const auto minutes = ch::duration_cast<ch::minutes>(midnight - hours);
        lua_pushinteger(L, minutes.count());
        return 1;
    }},
    {"second", [](lua_State* L, time_type& tp) -> int {
        const auto midnight = tp - ch::floor<ch::days>(tp);
        const auto hours = ch::duration_cast<ch::hours>(midnight);
        const auto minutes = ch::duration_cast<ch::minutes>(midnight - hours);
        const auto seconds = ch::duration_cast<ch::seconds>(midnight - hours - minutes);
        lua_pushinteger(L, seconds.count());
        return 1;
    }},
};

static int tostring(lua_State* L) {
    time_type& tp = lb::check_udata(L, 1);
    lua_pushstring(L, std::format("{:%x %X}", tp).c_str());
    return 1;
}

time_type& new_time(lua_State* L, const time_type& v) {
    if (not lb::initialized(L)) {
        const luaL_Reg meta[] = {
            {"__tostring", tostring},
            {nullptr, nullptr}
        };
        lb::init(L, {
            .index = index,
            .namecall = namecall,
            .meta = meta,
        });
    }
    return lb::new_udata(L, v);
}
