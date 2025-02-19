#include "time.hpp"
#include <format>
#include <chrono>
#include <string>
using namespace std::string_literals;
using namespace std::chrono;
using Registry = T_time_span::Registry;
using Init_info = T_time_span::Init_info;
using std::string, std::format;

static auto default_format(Time_span amount) -> string {
    auto h = duration_cast<hours>(amount);
    amount -= h;
    auto m = duration_cast<minutes>(amount);
    amount -= m;
    auto s = duration_cast<seconds>(amount);
    amount -= s;
    auto ns = amount;
    std::string result = format("{:02}:{:02}:{:02}.{:09}", h.count(),
        m.count(), s.count(), ns.count());
    return result;
}
static const Registry namecall = {
    {"format", [](lua_State* L, Time_span& d) -> int {
        const std::string fmt = "{:"s + luaL_checkstring(L, 2) + "}"s;
        lua_pushstring(L, std::vformat(fmt, std::make_format_args(d)).c_str());
        return 1;
    }},
};

static const Registry indices = {
    {"total_seconds", [](lua_State* L, Time_span& self) -> int {
        lua_pushnumber(L, duration<double>(self).count());
        return 1;
    }},{"total_microseconds", [](lua_State* L, Time_span& self) -> int {
        lua_pushnumber(L, duration<double, std::micro>(self).count());
        return 1;
    }},{"total_nanoseconds", [](lua_State* L, Time_span& self) -> int {
        lua_pushnumber(L, duration<double, std::nano>(self).count());
        return 1;
    }},{"total_minutes", [](lua_State* L, Time_span& self) -> int {
        lua_pushnumber(L, duration<double, std::ratio<60>>(self).count());
        return 1;
    }},{"total_hours", [](lua_State* L, Time_span& self) -> int {
        lua_pushnumber(L, duration<double, std::ratio<60*60>>(self).count());
        return 1;
    }},
};

static auto sub(lua_State* L) -> int {
    if (T_time_point::is(L, 1)) {
        auto& p = T_time_point::check(L, 1);
        T_time_point::create(L, Time_point(
            p.get_time_zone(),
            p.get_sys_time() - duration_cast<milliseconds>(T_time_span::check(L, 2))
        ));
        return 1;
    } else if (T_nano_time_point::is(L, 1)) {
        T_nano_time_point::make(L, T_nano_time_point::check(L, 1) - T_time_span::check(L, 2));
        return 1;
    } else if (T_time_span::is(L, 1) and T_time_span::is(L, 2)) {
        T_time_span::make(L) = T_time_span::check(L, 1) + T_time_span::check(L, 2);
        return 1;
    }
    luaL_errorL(L, "unknown arithmetic operation");
}
static auto add(lua_State* L) -> int {
    if (T_time_point::is(L, 1)) {
        auto& p = T_time_point::check(L, 1);
        T_time_point::create(L, Time_point(
            p.get_time_zone(),
            p.get_sys_time() + duration_cast<milliseconds>(T_time_span::check(L, 2))
        ));
        return 1;
    } else if (T_nano_time_point::is(L, 1)) {
        T_nano_time_point::create(L, T_nano_time_point::check(L, 1) + T_time_span::check(L, 2));
        return 1;
    } else if (T_time_span::is(L, 1) and T_time_span::is(L, 2)) {
        T_time_span::create(L, T_time_span::check(L, 1) + T_time_span::check(L, 2));
        return 1;
    }
    luaL_errorL(L, "unknown arithmetic operation");
}

static auto tostring(lua_State* L) -> int {
    Time_span& p = T_time_span::check(L, 1);
    lua_pushstring(L, default_format(p).c_str());
    return 1;
}
constexpr luaL_Reg meta[] = {
    {"__tostring", tostring},
    {"__sub", sub},
    {"__add", add},
    {nullptr, nullptr}
};
template<> const Init_info T_time_span::init_info {
    .index = indices,
    .namecall = namecall,
    .meta = meta,
};
