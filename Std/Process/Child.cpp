#include "process.hpp"

static const T_child::Registry index {
    {"pid", [](lua_State* L, Child& child) {
        T_pid::make(L, child.id());
        return 1;
    }},
    {"joinable", [](lua_State* L, Child& child) {
        dluau::push(L, child.joinable());
        return 1;
    }},
    {"exit_code", [](lua_State* L, Child& child) {
        dluau::push(L, child.exit_code());
        return 1;
    }},
    {"native_exit_code", [](lua_State* L, Child& child) {
        dluau::push(L, child.native_exit_code());
        return 1;
    }},
    {"running", [](lua_State* L, Child& child) {
        dluau::push(L, child.running());
        return 1;
    }},
};
static const T_child::Registry namecall {
    {"join", [](lua_State* L, Child& child) {
        child.join();
        return 0;
    }},
    {"terminate", [](lua_State* L, Child& child) {
        child.terminate();
        return 0;
    }},
    {"detach", [](lua_State* L, Child& child) {
        child.detach();
        return 0;
    }},
    {"wait", [](lua_State* L, Child& child) {
        child.wait();
        return 0;
    }},
};

template <> const T_child::Init_info T_child::init_info {
    .index = index,
    .namecall = namecall,
};
