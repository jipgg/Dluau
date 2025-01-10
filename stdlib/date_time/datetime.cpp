#include "date_time.hpp"
#include <lumin.h>
#include <format>
#include <chrono>
#include <unordered_map>
#include "lazy_type_utils.hpp"
using tutils = lazy_type_utils<datetime>;
using namecall_function = int(*)(lua_State*, datetime&);
using index_function = int(*)(lua_State*, datetime&, std::string_view);
static std::unordered_map<int, namecall_function> namecalls{};

datetime* todatetime(lua_State* L, int idx) {
    return &tutils::check(L, idx);
}

static const tutils::map index = {
    {"year", [](lua_State* L, datetime& tp) -> int {
        ch::year_month_day ymd{ch::floor<ch::days>(tp)};
        lua_pushinteger(L, static_cast<int>(ymd.year()));
        return 1;
    }},
    {"month", [](lua_State* L, datetime& tp) -> int {
        ch::year_month_day ymd{ch::floor<ch::days>(tp)};
        lua_pushinteger(L, static_cast<unsigned int>(ymd.month()));
        return 1;
    }},
    {"day", [](lua_State* L, datetime& tp) -> int {
        ch::year_month_day ymd{ch::floor<ch::days>(tp)};
        lua_pushinteger(L, static_cast<unsigned int>(ymd.day()));
        return 1;
    }},
    {"hour", [](lua_State* L, datetime& tp) -> int {
        const auto midnight = tp - ch::floor<ch::days>(tp);
        const auto hours = ch::duration_cast<ch::hours>(midnight);
        lua_pushinteger(L, hours.count());
        return 1;
    }},
    {"minute", [](lua_State* L, datetime& tp) -> int {
        const auto midnight = tp - ch::floor<ch::days>(tp);
        const auto hours = ch::duration_cast<ch::hours>(midnight);
        const auto minutes = ch::duration_cast<ch::minutes>(midnight - hours);
        lua_pushinteger(L, minutes.count());
        return 1;
    }},
    {"second", [](lua_State* L, datetime& tp) -> int {
        const auto midnight = tp - ch::floor<ch::days>(tp);
        const auto hours = ch::duration_cast<ch::hours>(midnight);
        const auto minutes = ch::duration_cast<ch::minutes>(midnight - hours);
        const auto seconds = ch::duration_cast<ch::seconds>(midnight - hours - minutes);
        lua_pushinteger(L, seconds.count());
        return 1;
    }},
};

static int tostring(lua_State* L) {
    datetime& tp = tutils::check(L, 1);
    lua_pushstring(L, std::format("{}", tp).c_str());
    return 1;
}

datetime& newdatetime(lua_State* L, const datetime& v) {
    if (not tutils::initialized()) {
        const luaL_Reg meta[] = {
            {"__tostring", tostring},
            {nullptr, nullptr}
        };
        tutils::init(L, "datetime", index, {}, {}, meta);
    }
    return tutils::create(L, v);
}
