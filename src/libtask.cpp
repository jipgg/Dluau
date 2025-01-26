#include "dluau.h"
#include "shared.hpp"
#include <common.hpp>
#include <chrono>
#include <queue>
#include <variant>
#include <boost/container/flat_map.hpp>
using namespace std::chrono_literals;
namespace chr = std::chrono;
using chr::steady_clock;
using stime_point = steady_clock::time_point;
using chr::milliseconds, chr::duration;
using std::variant, std::queue;
using std::optional, std::nullopt;
using std::vector;
using boost::container::flat_map;
using common::error_trail, common::raii;
using lthread = lua_State*;

struct waiter {
    stime_point start;
    duration<double> length;
    int ref;
};
enum class job {
    waiter, cleaner
};
struct cleaning_instruction {
    lthread state;
    job job;
};

static flat_map<lthread, waiter> waiting;
static vector<cleaning_instruction> janitor;

static int wait(lua_State* L) {
    if (not lua_isyieldable(L)) luaL_errorL(L, "current context is not yieldable");
    auto now = steady_clock::now(); 
    if (waiting.contains(L)) {
        waiting[L].start = now;
        waiting[L].length = duration<double>(luaL_optnumber(L, 1, 0.001));
        return lua_yield(L, 0);
    }
    auto length = duration<double>(luaL_checknumber(L, 1));
    lua_pushthread(L);
    int ref = lua_ref(L, -1);
    waiting.emplace(L, waiter{
        .start = std::move(now),
        .length = std::move(length),
        .ref = ref,
    });
    return lua_yield(L, 0);
}

namespace shared {
bool tasks_in_progress() {
    return not waiting.empty();
}
optional<error_trail> task_step(lua_State* L) {
    const auto now = steady_clock::now();
    for (auto& [state, waiter] : waiting) {
        const auto old_start = waiter.start;
        if (now - waiter.start < waiter.length) continue;
        const int status = lua_resume(state, L, 0);
        const bool yielded_independently = waiter.start == old_start;
        if (status == LUA_OK or yielded_independently) {
            janitor.emplace_back(state, job::waiter);
            continue;
        } else if (status == LUA_YIELD) {
        } else return error_trail(lua_tostring(state, -1));
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
        {nullptr, nullptr}
    };
    luaL_register(L, "task", lib);
}
