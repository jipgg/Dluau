#include "date_time.hpp"
#include <lumin.h>
#include <format>
#include <chrono>
#include <unordered_map>
using namecall_function = int(*)(lua_State*, duration&);
using index_function = int(*)(lua_State*, duration&, std::string_view);
static std::unordered_map<int, namecall_function> namecalls{};
static const int tag = lumin_newtypetag();
constexpr const char* tname = "duration";

duration* toduration(lua_State* L, int idx) {
    return static_cast<duration*>(lua_touserdatatagged(L, idx, tag));
}

static std::unordered_map<std::string_view, index_function> indices {
    {"seconds", [](lua_State* L, duration& self, std::string_view key) -> int {
        lua_pushnumber(L, ch::duration<double>(self).count());
        return 1;
    }},
    {"microseconds", [](lua_State* L, duration& self, std::string_view key) -> int {
        lua_pushnumber(L, ch::duration<double, std::micro>(self).count());
        return 1;
    }},
    {"nanoseconds", [](lua_State* L, duration& self, std::string_view key) -> int {
        lua_pushnumber(L, ch::duration<double, std::nano>(self).count());
        return 1;
    }},
    {"minutes", [](lua_State* L, duration& self, std::string_view key) -> int {
        lua_pushnumber(L, ch::duration<double, std::ratio<60>>(self).count());
        return 1;
    }},
    {"hours", [](lua_State* L, duration& self, std::string_view key) -> int {
        lua_pushnumber(L, ch::duration<double, std::ratio<60*60>>(self).count());
        return 1;
    }},
};

static int index(lua_State* L) {
    duration& tp = *toduration(L, 1);
    const std::string_view key = luaL_checkstring(L, 2); 
    auto found_it = indices.find(key);
    if (found_it == indices.end()) luaL_errorL(L, "invalid index");
    return found_it->second(L, tp, key);
}
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

static int namecall(lua_State* L) {
    duration* p = toduration(L, 1);
    int atom;
    lua_namecallatom(L, &atom);
    auto found_it = namecalls.find(atom);
    if (found_it == namecalls.end()) luaL_errorL(L, "invalid namecall");
    return found_it->second(L, *p);
}
static int tostring(lua_State* L) {
    duration* p = toduration(L, 1);
    lua_pushstring(L, std::format("{}", *p).c_str());
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

duration& newduration(lua_State* L, const duration& v) {
    if (luaL_newmetatable(L, tname)) {
        static const luaL_Reg meta[] = {
            {"__namecall", namecall},
            {"__tostring", tostring},
            {"__index", index},
            {"__sub", sub},
            {"__add", add},
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
    duration* p = static_cast<duration*>(lua_newuserdatatagged(L, sizeof(duration), tag));
    luaL_getmetatable(L, tname);
    lua_setmetatable(L, -2);
    new (p) duration{v};
    return *p;
}

void register_duration(lua_State *L) {
    const luaL_Reg ctors[] = {
        {"create", [](lua_State* L) -> int {
            const int length = luaL_checkinteger(L, 1);
            const std::string_view t = luaL_optstring(L, 2, "s");
            if (t == "ns") newduration(L, ch::nanoseconds(length));
            else if (t == "ms") newduration(L, ch::milliseconds(length));
            else if (t == "us") newduration(L, ch::microseconds(length));
            else if (t == "min") newduration(L, ch::minutes(length));
            else if (t == "h") newduration(L, ch::hours(length));
            else newduration(L, ch::seconds(length));
            return 1;
        }},
        {"seconds", [](lua_State* L) -> int {
            newduration(L, ch::seconds{luaL_checkinteger(L, 1)});
            return 1;
        }},
        {"nanoseconds", [](lua_State* L) -> int {
            newduration(L, ch::nanoseconds{luaL_checkinteger(L, 1)});
            return 1;
        }},
        {"microseconds", [](lua_State* L) -> int {
            newduration(L, ch::microseconds{luaL_checkinteger(L, 1)});
            return 1;
        }},
        {"minutes", [](lua_State* L) -> int {
            newduration(L, ch::minutes{luaL_checkinteger(L, 1)});
            return 1;
        }},
        {"hours", [](lua_State* L) -> int {
            newduration(L, ch::hours{luaL_checkinteger(L, 1)});
            return 1;
        }},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, ctors);
    lua_setfield(L, -2, "duration");
}
