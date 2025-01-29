#include "dluau.h"
#include "shared.hpp"
#include <common.hpp>
#include <chrono>
#include <queue>
#include <variant>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <utility>
#include <ranges>
#include <algorithm>
using namespace std::chrono_literals;
namespace chr = std::chrono;
namespace rngs = std::ranges;
using chr::steady_clock;
using stime_point = steady_clock::time_point;
using chr::milliseconds, chr::duration;
using std::variant, std::queue;
using std::optional, std::nullopt;
using std::vector, std::tuple;
using boost::container::flat_map;
using boost::container::flat_set;
using common::error_trail, common::raii;
using luathread = lua_State*;

struct deferrer {
    lua_State* state;
    int ref;
    int argn;
};

struct waiter {
    stime_point start;
    duration<double> length;
    int argn;
    int ref;
};
enum class job {
    waiter, cleaner
};
struct cleaning_instruction {
    luathread state;
    job job;
};

static flat_map<luathread, waiter> waiting;
static vector<deferrer> deferred;
static vector<cleaning_instruction> janitor;

static bool has_errored(lua_State* state) {
    const int status = lua_status(state);
    return status != LUA_OK and status != LUA_YIELD;
}
static bool has_errored(int status) {
    return status != LUA_OK and status != LUA_YIELD;
}

static int wait(lua_State* L) {
    if (not lua_isyieldable(L)) luaL_errorL(L, "current context is not yieldable");
    const auto now = steady_clock::now(); 
    const auto sec = duration<double>(luaL_optnumber(L, 1, 0));
    if (waiting.contains(L)) {
        waiting[L].start = now;
        waiting[L].length = sec;
        return lua_yield(L, 0);
    }
    lua_pushthread(L);
    int ref = lua_ref(L, -1);
    waiting.emplace(L, waiter{
        .start = now,
        .length = sec,
        .argn = 0,
        .ref = ref,
    });
    return lua_yield(L, 0);
}
static tuple<lua_State*, int> resolve_task_argument(lua_State* L, int idx) {
    lua_State* state = nullptr;
    const int top = lua_gettop(L);
    if (lua_isfunction(L, idx)) {
        state = lua_newthread(L);
        lua_pushvalue(L, idx);
        lua_xmove(L, state, 1);
    } else if (lua_isthread(L, idx)) {
        state = lua_tothread(L, idx);
        lua_pushvalue(L, idx);
    }
    if (state) {
        for (int i{idx + 1}; i <= top; ++i) lua_pushvalue(L, i);
        lua_xmove(L, state, top - idx);
    }
    return {state, top - idx};
}
static int spawn(lua_State* L) {
    auto [state, argn] = resolve_task_argument(L, 1);
    if (not state) luaL_typeerrorL(L, 1, "function or thread");
    const int status = lua_resume(state, L, argn);
    if (has_errored(status)) {
        lua_xmove(state, L, 1);
        lua_error(L);
    }
    return 1;
}
static int delay(lua_State* L) {
    const auto now = steady_clock::now();
    const auto sec = duration<double>(luaL_optnumber(L, 1, 0));
    auto [state, argn] = resolve_task_argument(L, 2);
    if (not state) luaL_typeerrorL(L, 1, "function or thread");
    waiting.emplace(state, waiter{
        .start = now,
        .length = sec,
        .argn = argn,
        .ref = lua_ref(L, -1),
    });
    return 1;
}
static int defer(lua_State* L) {
    auto [state, argn] = resolve_task_argument(L, 1);
    if (not state) luaL_typeerrorL(L, 1, "function or thread");
    deferred.push_back(deferrer{
        .state = state,
        .ref = lua_ref(L, -1),
        .argn = argn,
    });
    return 1;
}
static int cancel(lua_State* L) {
    lua_State* state = lua_tothread(L, 1);
    if (waiting.contains(state)) {
        lua_unref(L, waiting[state].ref);
        waiting.erase(state);
        return 0;
    }
    auto in_deferred = rngs::find_if(deferred, [&state](deferrer& e) {
        return e.state == state;
    });
    if (in_deferred != rngs::end(deferred)) {
        lua_unref(L, in_deferred->ref);
        deferred.erase(in_deferred);
        return 0;
    }
    return 0;
}

namespace shared {
bool tasks_in_progress() {
    return not (waiting.empty() and deferred.empty());
}
optional<error_trail> task_step(lua_State* L) {
    const auto now = steady_clock::now();
    for (auto& d : deferred) {
        auto status = lua_resume(d.state, L, d.argn);
        lua_unref(L, d.ref);
        if (has_errored(status)) {
            return error_trail(lua_tostring(d.state, -1));
        }
    }
    deferred.clear();
    for (auto& [state, waiter] : waiting) {
        const auto old_start = waiter.start;
        if (now - waiter.start < waiter.length) continue;
        const int status = lua_resume(state, L, waiter.argn);
        const bool yielded_independently{
            status == LUA_YIELD
            and waiter.start == old_start
        };
        if (status == LUA_OK or yielded_independently) {
            janitor.emplace_back(state, job::waiter);
            continue;
        } else if (status != LUA_YIELD){
            return error_trail(lua_tostring(state, -1));
        }
    }
    for (const auto& v : janitor) {
        switch (v.job) {
            case job::waiter:
                lua_unref(L, waiting.at(v.state).ref);
                waiting.erase(v.state);
                continue;
            case job::cleaner:
                continue;
        }
    }
    janitor.clear();
    return nullopt;
}
}

void dluauopen_task(lua_State* L) {
    const luaL_Reg lib[] = {
        {"wait", wait},
        {"spawn", spawn},
        {"defer", defer},
        {"delay", delay},
        {"cancel", cancel},
        {nullptr, nullptr}
    };
    luaL_register(L, "task", lib);
}
