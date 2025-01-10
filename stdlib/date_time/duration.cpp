#include "date_time.hpp"
#include <lumin.h>
#include <format>
#include <chrono>
#include <unordered_map>
#include "lazy_type_utils.hpp"
using ltu = lazy_type_utils<duration>;
using namecall_function = int(*)(lua_State*, duration&);
using index_function = int(*)(lua_State*, duration&, std::string_view);
static std::unordered_map<int, namecall_function> namecalls{};
constexpr const char* tname = "duration";

duration* toduration(lua_State* L, int idx) {
    return &ltu::check(L, idx);
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

    // Format the result
    std::string result = std::format("{:02}:{:02}:{:02}.{:09}",
                                     hours.count(),
                                     minutes.count(),
                                     seconds.count(),
                                     nanoseconds.count());
    return result;
}

static const ltu::map indices = {
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
    const int tag1 = lua_userdatatag(L, 1);
    const int tag2 = lua_userdatatag(L, 2);
    if (tag1 == tag2) {
        newduration(L, *toduration(L, 1) - *toduration(L, 2));
        return 1;
    }
    luaL_errorL(L, "unknown arithmetic operation");
}
static int add(lua_State* L) {
    const int tag1 = lua_userdatatag(L, 1);
    const int tag2 = lua_userdatatag(L, 2);
    if (tag1 == tag2) {
        newduration(L, *toduration(L, 1) + *toduration(L, 2));
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
    if (not ltu::initialized()) {
        const luaL_Reg meta[] = {
            {"__tostring", tostring},
            {"__sub", sub},
            {"__add", add},
            {nullptr, nullptr}
        };
        ltu::init(L, "duration", indices, {}, {}, meta);
    }
    return ltu::create(L, v);
}

void register_duration(lua_State *L) {
}
