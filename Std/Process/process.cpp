#include "process.hpp"
#include <filesystem>
#include <lua_utility.hpp>
#include <format>
#include <variant>
namespace fs = std::filesystem;
using std::variant;
using std::vector;
using std::string;
struct Resolved_proc_info {
    string exe;
    vector<string> args;
};

static auto resolve_procinfo(lua_State* L) -> variant<string, Resolved_proc_info> {
    const int top = lua_gettop(L);
    if (top > 1) {
        vector<string> args{};
        args.reserve(top - 1);
        for (int i{2}; i <= top; ++i) {
            args.emplace_back(luaL_checkstring(L, i));
        }
        string exe{luaL_checkstring(L, 1)};
        if (not fs::exists(exe)) exe = bp::search_path(exe).string();
        return Resolved_proc_info{.exe = std::move(exe), .args = std::move(args)};
    } else {
        return string(luaL_checkstring(L, 1));
    }
}

static auto system(lua_State* L) -> int {
    try {
        auto variant = resolve_procinfo(L);
        if (auto* shell_cmd = std::get_if<string>(&variant)) {
            lu::push(L, bp::system(bp::shell, luaL_checkstring(L, 1)));
            return 1;
        } else {
            auto& info = std::get<Resolved_proc_info>(variant);
            lu::push(L, bp::system(bp::exe = info.exe, bp::args = info.args));
            return 1;

        }
    } catch (bp::process_error& e) {
        lu::error(L, "{}", e.what());
    }
}
static auto search_path(lua_State* L) -> int {
    std::string path = bp::search_path(luaL_checkstring(L, 1)).string();
    if (path.empty()) return 0;
    lu::push(L, path);
    return 1;
}
static auto spawn(lua_State* L) -> int {
    try {
        auto variant = resolve_procinfo(L);
        if (auto* shell_cmd = std::get_if<string>(&variant)) {
            bp::spawn(bp::shell, luaL_checkstring(L, 1));
            return 0;
        } else {
            auto& info = std::get<Resolved_proc_info>(variant);
            bp::spawn(bp::exe = info.exe, bp::args = info.args);
            return 0;
        }
    } catch (bp::process_error& e) {
        lu::error(L, "{}", e.what());
    }
}
static auto child_ctor(lua_State* L) -> int {
    try {
        if (T_pid::is(L, 1)) {
            T_child::make(L, T_pid::check(L, 1));
            return 1;
        }
        auto var = resolve_procinfo(L);
        if (auto* cmd = std::get_if<string>(&var)) {
            T_child::make(L, bp::shell, *cmd);
            return 1;
        } else {
            auto& info = std::get<Resolved_proc_info>(var);
            T_child::make(L, bp::child(bp::exe = info.exe, bp::args = info.args));
            return 1;
        }
    } catch(bp::process_error& e) {
        lu::error(L, e.what());
    }
}

DLUAUSTD_API auto dlrequire(lua_State* L) -> int {
    lua_newtable(L);
    constexpr luaL_Reg lib[] = {
        {"system", system},
        {"search_path", search_path},
        {"spawn", spawn},
        {"child", child_ctor},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, lib);
    lua_setreadonly(L, -1, true);
    return 1;
}
