#include "process.hpp"
#include <string>

constexpr luaL_Reg meta[] = {
    {"__tostring", [](lua_State* L) {
        auto& id = T_pid::check(L, 1);
        dluau::push(L, std::to_string(id));
        return 1;
    }},
    {"__eq", [](lua_State* L) {
        auto& a = T_pid::check(L, 1);
        auto& b = T_pid::check(L, 2);
        dluau::push(L, a == b);
        return 1;
    }},
    {nullptr, nullptr}
};

template <> const T_pid::Init_info T_pid::init_info {
    .meta = meta,
};
