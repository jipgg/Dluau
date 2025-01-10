#include "date_time.hpp"
#include <lumin.h>
#include <format>
#include <chrono>
#include "userdata_lazybuilder.hpp"
using lb = userdata_lazybuilder<duration>;
template<> const char* lb::type_name(){return "duration";}

duration* toduration(lua_State* L, int idx) {
    return &lb::check_udata(L, idx);
}
static std::string default_format(duration amount) {
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

static const lb::registry indices = {
    {"seconds", [](lua_State* L, duration& self) -> int {
        lua_pushnumber(L, ch::duration<double>(self).count());
        return 1;
    }},{"microseconds", [](lua_State* L, duration& self) -> int {
        lua_pushnumber(L, ch::duration<double, std::micro>(self).count());
        return 1;
    }},{"nanoseconds", [](lua_State* L, duration& self) -> int {
        lua_pushnumber(L, ch::duration<double, std::nano>(self).count());
        return 1;
    }},{"minutes", [](lua_State* L, duration& self) -> int {
        lua_pushnumber(L, ch::duration<double, std::ratio<60>>(self).count());
        return 1;
    }},{"hours", [](lua_State* L, duration& self) -> int {
        lua_pushnumber(L, ch::duration<double, std::ratio<60*60>>(self).count());
        return 1;
    }},
};

static int sub(lua_State* L) {
    if (lb::is_type(L, 1) and lb::is_type(L, 2)) {
        lb::new_udata(L, *toduration(L, 1) - *toduration(L, 2));
        return 1;
    }
    luaL_errorL(L, "unknown arithmetic operation");
}
static int add(lua_State* L) {
    if (lb::is_type(L, 1) and lb::is_type(L, 2)) {
        lb::new_udata(L, *toduration(L, 1) + *toduration(L, 2));
        return 1;
    }
    luaL_errorL(L, "unknown arithmetic operation");
}

static int tostring(lua_State* L) {
    duration* p = toduration(L, 1);
    lua_pushstring(L, default_format(*p).c_str());
    return 1;
}
duration& newduration(lua_State* L, const duration& v) {
    if (not lb::initialized(L)) {
        const luaL_Reg meta[] = {
            {"__tostring", tostring},
            {"__sub", sub},
            {"__add", add},
            {nullptr, nullptr}
        };
        lb::init(L, {
            .index = indices,
            .meta = meta,
        });
    }
    return lb::new_udata(L, v);
}

void register_duration(lua_State *L) {
}
