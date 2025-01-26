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
    raii ref;
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
    if (waiting.contains(L)) {
        std::cout << std::format("WAITING AGAIN {}\n", luaL_checknumber(L, 1));
        waiting[L].length += duration<double>(luaL_checknumber(L, 1));
        return lua_yield(L, 0);
    }
    auto now = steady_clock::now(); 
    auto length = duration<double>(luaL_checknumber(L, 1));
    lua_pushthread(L);
    lua_State* main_thread = lua_mainthread(L);
    int ref = lua_ref(main_thread, -1);
        std::cout << std::format("WAITING {}\n", ref);
    raii dtor([ref, main_thread] {
        std::cout << std::format("removing reference {}\n", ref);
        lua_unref(main_thread, ref);
    });
    waiting.emplace(L, waiter{
        .start = std::move(now),
        .length = std::move(length),
        .ref = std::move(dtor),
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
        const auto old_length = waiter.length;
        if (now - waiter.start < waiter.length) continue;
        const int status = lua_resume(state, L, 0);
        const bool yielded_independently = waiter.length == old_length;
        if (status == LUA_OK or yielded_independently) {
            janitor.emplace_back(state, job::waiter);
        } else if (status != LUA_YIELD) {
            return error_trail(lua_tostring(state, -1));
        }
    }
    for (const auto& v : janitor) {
        switch (v.job) {
            case job::waiter:
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
