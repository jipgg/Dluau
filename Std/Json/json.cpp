#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <print>
#include <span>
#include <nlohmann/json.hpp>
#include "json.hpp"

#include <iostream>
using std::string, std::string_view;
using nlohmann::json;
using Json_value = decltype(json::parse(""));

static auto push_value(lua_State* L,  const Json_value& val) -> int {
    using Val_t = json::value_t;
    switch (val.type()) {
        case Val_t::string:
            dluau::push(L, string(val));
        return 1;
        case Val_t::boolean:
            dluau::push(L, bool(val));
        return 1;
        case Val_t::null:
            lua_pushnil(L);
        return 1;
        case Val_t::number_float:
        case Val_t::number_integer:
        case Val_t::number_unsigned:
            lua_pushnumber(L, double(val));
        return 1;
        case Val_t::binary: {
            const auto& vec = val.get_binary();
            uint8_t* buf = static_cast<uint8_t*>(lua_newbuffer(L, vec.size()));
            std::memcpy(buf, vec.data(), vec.size());
        } return 1;
        case Val_t::discarded:
            dluau::error(L, "discarded value");
        break;
        case Val_t::array: {
            lua_newtable(L);
            int idx{1};
            for (const auto& subval : val) {
                push_value(L, subval);
                lua_rawseti(L, -2, idx++);
            }
        } return 1;
        case Val_t::object: {
            lua_newtable(L);
            for (const auto& [key, subval] : val.items()) {
                push_value(L, subval);
                lua_rawsetfield(L, -2, string(key).c_str());
            }
        } return 1;
    }
    return 0;
}

static auto parse(lua_State* L) -> int {
    try {
        auto parsed = json::parse(luaL_checkstring(L, 1));
        push_value(L, parsed);
        return 1;
    } catch (json::exception& e) {
        dluau::arg_error(L, 1, e.what());
    }
}

DLUAUSTD_API auto dlrequire(lua_State* L) -> int {
    const luaL_Reg lib[] = {
        {"parse", parse},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    return 1;
}
