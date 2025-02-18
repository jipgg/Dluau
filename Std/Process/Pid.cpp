#include "process.hpp"
#include <lua_utility.hpp>
#include <string>

constexpr luaL_Reg meta[] = {
    {"__tostring", [](lua_State* L) {
        auto& id = PidType::check(L, 1);
        lu::push(L, std::to_string(id));
        return 1;
    }},
    {"__eq", [](lua_State* L) {
        auto& a = PidType::check(L, 1);
        auto& b = PidType::check(L, 2);
        lu::push(L, a == b);
        return 1;
    }},
    {nullptr, nullptr}
};

template <> const PidType::InitInfo PidType::init_info {
    .meta = meta,
};
