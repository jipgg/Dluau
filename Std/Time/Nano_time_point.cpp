#include "time.hpp"
#include <format>
#include <chrono>
#include <string>
using namespace std::string_literals;
namespace sc = std::chrono;
using sc::days, sc::hours, sc::minutes, sc::seconds,
sc::nanoseconds, sc::milliseconds, sc::microseconds;
using Registry = T_nano_time_point::Registry;
using Init_info = T_nano_time_point::Init_info;

static const Registry index = {
    {"hour", [](lua_State* L, Nano_time_point& tp) -> int {
        const auto midnight = tp - floor<days>(tp);
        const auto hrs = duration_cast<hours>(midnight);
        lua_pushinteger(L, hrs.count());
        return 1;
    }},
    {"minute", [](lua_State* L, Nano_time_point& tp) -> int {
        const auto midnight = tp - floor<days>(tp);
        const auto hrs = duration_cast<hours>(midnight);
        const auto min = duration_cast<minutes>(midnight - hrs);
        lua_pushinteger(L, min.count());
        return 1;
    }},
    {"second", [](lua_State* L, Nano_time_point& tp) -> int {
        const auto midnight = tp - floor<days>(tp);
        const auto hrs = duration_cast<hours>(midnight);
        const auto min = duration_cast<minutes>(midnight - hrs);
        const auto sec = duration_cast<seconds>(midnight - hrs - min);
        lua_pushinteger(L, sec.count());
        return 1;
    }},
    {"millisecond", [](lua_State* L, Nano_time_point& tp) -> int {
        const auto midnight = tp - floor<days>(tp);
        const auto hrs = duration_cast<hours>(midnight);
        const auto min = duration_cast<minutes>(midnight - hrs);
        const auto sec = duration_cast<seconds>(midnight - hrs - min);
        const auto milli = duration_cast<milliseconds>(midnight - hrs - min - sec);
        lua_pushinteger(L, milli.count());
        return 1;
    }},
    {"microsecond", [](lua_State* L, Nano_time_point& tp) -> int {
        const auto midnight = tp - floor<days>(tp);
        const auto hrs = duration_cast<hours>(midnight);
        const auto min = duration_cast<minutes>(midnight - hrs);
        const auto sec = duration_cast<seconds>(midnight - hrs - min);
        const auto milli = duration_cast<milliseconds>(
            midnight - hrs - min - sec);
        const auto micro = duration_cast<microseconds>(
            midnight - hrs - min - sec - milli);
        lua_pushinteger(L, micro.count());
        return 1;
    }},
    {"nanosecond", [](lua_State* L, Nano_time_point& tp) -> int {
        const auto midnight = tp - floor<days>(tp);
        const auto hrs = duration_cast<hours>(midnight);
        const auto min = duration_cast<minutes>(midnight - hrs);
        const auto sec = duration_cast<seconds>(midnight - hrs - min);
        const auto milli = duration_cast<milliseconds>(
            midnight - hrs - min - sec);
        const auto micro = duration_cast<microseconds>(
            midnight - hrs - min - sec - milli);
        const auto nano = duration_cast<nanoseconds>(
            midnight - hrs - min - sec - milli);
        lua_pushinteger(L, nano.count());
        return 1;
    }},
};
static auto sub(lua_State* L) -> int {
    T_time_span::make(L, T_nano_time_point::check(L, 1) - T_nano_time_point::check(L, 2));
    return 1;
}

static auto tostring(lua_State* L) -> int {
    Nano_time_point& tp = T_nano_time_point::check(L, 1);
    lua_pushstring(L, std::format("{}", tp.time_since_epoch()).c_str());
    return 1;
}

constexpr luaL_Reg meta[] = {
    {"__tostring", tostring},
    {"__sub", sub},
    {nullptr, nullptr}
};
template<> const Init_info T_nano_time_point::init_info {
    .index = index,
    .meta = meta,
};
