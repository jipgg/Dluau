#include "date_time.hpp"
#include <lumin.h>
#include <format>
#include <chrono>
#include <unordered_map>
using namecall_function = int(*)(lua_State*, datetime&);
using index_function = int(*)(lua_State*, datetime&, std::string_view);
static std::unordered_map<int, namecall_function> namecalls{};
static const int tag = lumin_newtypetag();
constexpr const char* tname = "datetime";

datetime* todatetime(lua_State* L, int idx) {
    return static_cast<datetime*>(lua_touserdatatagged(L, 1, tag));
}

static std::unordered_map<std::string_view, index_function> indices {
    {"year", [](lua_State* L, datetime& tp, std::string_view key) -> int {
        ch::year_month_day ymd{ch::floor<ch::days>(tp)};
        lua_pushinteger(L, static_cast<int>(ymd.year()));
        return 1;
    }},
    {"month", [](lua_State* L, datetime& tp, std::string_view key) -> int {
        ch::year_month_day ymd{ch::floor<ch::days>(tp)};
        lua_pushinteger(L, static_cast<unsigned int>(ymd.month()));
        return 1;
    }},
    {"day", [](lua_State* L, datetime& tp, std::string_view key) -> int {
        ch::year_month_day ymd{ch::floor<ch::days>(tp)};
        lua_pushinteger(L, static_cast<unsigned int>(ymd.day()));
        return 1;
    }},
    {"hour", [](lua_State* L, datetime& tp, std::string_view key) -> int {
        const auto midnight = tp - ch::floor<ch::days>(tp);
        const auto hours = ch::duration_cast<ch::hours>(midnight);
        lua_pushinteger(L, hours.count());
        return 1;
    }},
    {"minute", [](lua_State* L, datetime& tp, std::string_view key) -> int {
        const auto midnight = tp - ch::floor<ch::days>(tp);
        const auto hours = ch::duration_cast<ch::hours>(midnight);
        const auto minutes = ch::duration_cast<ch::minutes>(midnight - hours);
        lua_pushinteger(L, minutes.count());
        return 1;
    }},
    {"second", [](lua_State* L, datetime& tp, std::string_view key) -> int {
        const auto midnight = tp - ch::floor<ch::days>(tp);
        const auto hours = ch::duration_cast<ch::hours>(midnight);
        const auto minutes = ch::duration_cast<ch::minutes>(midnight - hours);
        const auto seconds = ch::duration_cast<ch::seconds>(midnight - hours - minutes);
        lua_pushinteger(L, seconds.count());
        return 1;
    }},
    {"second", [](lua_State* L, datetime& tp, std::string_view key) -> int {
        return 1;
    }},
};

static int index(lua_State* L) {
    datetime& tp = *todatetime(L, 1);
    const std::string_view key = luaL_checkstring(L, 2); 
    auto found_it = indices.find(key);
    if (found_it == indices.end()) luaL_errorL(L, "invalid index");
    return found_it->second(L, tp, key);
}

static int namecall(lua_State* L) {
    datetime* p = todatetime(L, 1);
    int atom;
    lua_namecallatom(L, &atom);
    auto found_it = namecalls.find(atom);
    if (found_it == namecalls.end()) luaL_errorL(L, "invalid namecall");
    return found_it->second(L, *p);
}
static int tostring(lua_State* L) {
    datetime& tp = *static_cast<datetime*>(lua_touserdatatagged(L, 1, tag));
    lua_pushstring(L, std::format("{}", tp).c_str());
    return 1;
}
static int add_namecall(lua_State* L, std::string_view key, namecall_function call) {
    lua_pushlstring(L, key.data(), key.size());
    int atom;
    lua_tostringatom(L, -1, &atom);
    lua_pop(L, 1);
    namecalls.emplace(atom, call);
    return atom;
}

datetime& newdatetime(lua_State* L, const datetime& v) {
    if (luaL_newmetatable(L, tname)) {
        static const luaL_Reg meta[] = {
            {"__namecall", namecall},
            {"__tostring", tostring},
            {"__index", index},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, tname);
        lua_setfield(L, -2, "__type");
        lua_setuserdatadtor(L, tag, [](lua_State* L, void* data) -> void {
            static_cast<datetime*>(data)->~datetime();
        });
    }
    lua_pop(L, 1);
    datetime* tp = static_cast<datetime*>(lua_newuserdatatagged(L, sizeof(datetime), tag));
    luaL_getmetatable(L, tname);
    lua_setmetatable(L, -2);
    new (tp) datetime{v};
    return *tp;
}
