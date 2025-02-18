#include "time.hpp"
#include <format>
#include <chrono>
#include <string>
using namespace std::string_literals;
using namespace std::chrono;

static std::string default_format(TimeSpan amount) {
    auto h = duration_cast<hours>(amount);
    amount -= h;
    auto m = std::chrono::duration_cast<std::chrono::minutes>(amount);
    amount -= m;
    auto s = std::chrono::duration_cast<std::chrono::seconds>(amount);
    amount -= s;
    auto ns = amount;
    std::string result = std::format("{:02}:{:02}:{:02}.{:09}", h.count(),
        m.count(), s.count(), ns.count());
    return result;
}
static const TimeSpanType::Registry namecall = {
    {"format", [](lua_State* L, TimeSpan& d) -> int {
        const std::string fmt = "{:"s + luaL_checkstring(L, 2) + "}"s;
        lua_pushstring(L, std::vformat(fmt, std::make_format_args(d)).c_str());
        return 1;
    }},
};

static const TimeSpanType::Registry indices = {
    {"total_seconds", [](lua_State* L, TimeSpan& self) -> int {
        lua_pushnumber(L, duration<double>(self).count());
        return 1;
    }},{"total_microseconds", [](lua_State* L, TimeSpan& self) -> int {
        lua_pushnumber(L, duration<double, std::micro>(self).count());
        return 1;
    }},{"total_nanoseconds", [](lua_State* L, TimeSpan& self) -> int {
        lua_pushnumber(L, duration<double, std::nano>(self).count());
        return 1;
    }},{"total_minutes", [](lua_State* L, TimeSpan& self) -> int {
        lua_pushnumber(L, duration<double, std::ratio<60>>(self).count());
        return 1;
    }},{"total_hours", [](lua_State* L, TimeSpan& self) -> int {
        lua_pushnumber(L, duration<double, std::ratio<60*60>>(self).count());
        return 1;
    }},
};

static int sub(lua_State* L) {
    if (TimeType::is(L, 1)) {
        auto& p = TimeType::check(L, 1);
        TimeType::create(L, Time(
            p.get_time_zone(),
            p.get_sys_time() - duration_cast<milliseconds>(TimeSpanType::check(L, 2))
        ));
        return 1;
    } else if (NanoTimeType::is(L, 1)) {
        NanoTimeType::make(L, NanoTimeType::check(L, 1) - TimeSpanType::check(L, 2));
        return 1;
    } else if (TimeSpanType::is(L, 1) and TimeSpanType::is(L, 2)) {
        TimeSpanType::make(L, TimeSpanType::check(L, 1) - TimeSpanType::check(L, 2));
        return 1;
    }
    luaL_errorL(L, "unknown arithmetic operation");
}
static int add(lua_State* L) {
    if (TimeType::is(L, 1)) {
        auto& p = TimeType::check(L, 1);
        TimeType::create(L, Time(
            p.get_time_zone(),
            p.get_sys_time() + duration_cast<milliseconds>(TimeSpanType::check(L, 2))
        ));
        return 1;
    } else if (NanoTimeType::is(L, 1)) {
        NanoTimeType::create(L, NanoTimeType::check(L, 1) + TimeSpanType::check(L, 2));
        return 1;
    } else if (TimeSpanType::is(L, 1) and TimeSpanType::is(L, 2)) {
        TimeSpanType::create(L, TimeSpanType::check(L, 1) + TimeSpanType::check(L, 2));
        return 1;
    }
    luaL_errorL(L, "unknown arithmetic operation");
}

static int tostring(lua_State* L) {
    TimeSpan& p = TimeSpanType::check(L, 1);
    lua_pushstring(L, default_format(p).c_str());
    return 1;
}
constexpr luaL_Reg meta[] = {
    {"__tostring", tostring},
    {"__sub", sub},
    {"__add", add},
    {nullptr, nullptr}
};
template<> const TimeSpanType::InitInfo TimeSpanType::init_info {
    .index = indices,
    .namecall = namecall,
    .meta = meta,
};
