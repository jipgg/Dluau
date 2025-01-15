#include "module.hpp"
#include <lumin.h>
#include <format>
#include <chrono>
#include "Userdata_template.hpp"
#include <string>
using namespace std::string_literals;
using Duration_ut = Userdata_template<Duration>;
static const std::string tname = module_name + "."s + "Duration";
template<> const char* Duration_ut::type_name(){return tname.c_str();}

Duration* to_duration(lua_State* L, int idx) {
    return &Duration_ut::check_udata(L, idx);
}
static std::string default_format(Duration amount) {
    using ch::duration_cast;
    auto hours = duration_cast<ch::hours>(amount);
    amount -= hours;
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(amount);
    amount -= minutes;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(amount);
    amount -= seconds;
    auto nanoseconds = amount;
    std::string result = std::format("{:02}:{:02}:{:02}.{:09}", hours.count(),
        minutes.count(), seconds.count(), nanoseconds.count());
    return result;
}
static const Duration_ut::Registry namecall = {
    {"format", [](lua_State* L, Duration& d) -> int {
        const std::string fmt = "{:"s + luaL_checkstring(L, 2) + "}"s;
        lua_pushstring(L, std::vformat(fmt, std::make_format_args(d)).c_str());
        return 1;
    }},
    {"type", [](lua_State* L, Duration& d) -> int {
        lua_pushstring(L, tname.c_str());
        return 1;
    }},
};

static const Duration_ut::Registry indices = {
    {"total_seconds", [](lua_State* L, Duration& self) -> int {
        lua_pushnumber(L, ch::duration<double>(self).count());
        return 1;
    }},{"total_microseconds", [](lua_State* L, Duration& self) -> int {
        lua_pushnumber(L, ch::duration<double, std::micro>(self).count());
        return 1;
    }},{"total_nanoseconds", [](lua_State* L, Duration& self) -> int {
        lua_pushnumber(L, ch::duration<double, std::nano>(self).count());
        return 1;
    }},{"total_minutes", [](lua_State* L, Duration& self) -> int {
        lua_pushnumber(L, ch::duration<double, std::ratio<60>>(self).count());
        return 1;
    }},{"total_hours", [](lua_State* L, Duration& self) -> int {
        lua_pushnumber(L, ch::duration<double, std::ratio<60*60>>(self).count());
        return 1;
    }},
};

static int sub(lua_State* L) {
    if (Duration_ut::is_type(L, 1) and Duration_ut::is_type(L, 2)) {
        Duration_ut::new_udata(L, *to_duration(L, 1) - *to_duration(L, 2));
        return 1;
    }
    luaL_errorL(L, "unknown arithmetic operation");
}
static int add(lua_State* L) {
    if (Duration_ut::is_type(L, 1) and Duration_ut::is_type(L, 2)) {
        Duration_ut::new_udata(L, *to_duration(L, 1) + *to_duration(L, 2));
        return 1;
    }
    luaL_errorL(L, "unknown arithmetic operation");
}

static int tostring(lua_State* L) {
    Duration* p = to_duration(L, 1);
    lua_pushstring(L, default_format(*p).c_str());
    return 1;
}
Duration& new_duration(lua_State* L, const Duration& v) {
    if (not Duration_ut::initialized(L)) {
        lumin_adduserdatatype(tname.c_str());
        const luaL_Reg meta[] = {
            {"__tostring", tostring},
            {"__sub", sub},
            {"__add", add},
            {nullptr, nullptr}
        };
        Duration_ut::init(L, {
            .index = indices,
            .namecall = namecall,
            .meta = meta,
        });
    }
    return Duration_ut::new_udata(L, v);
}

