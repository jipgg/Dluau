#include <dluau.hpp>
#include <format>
#include "luacode.h"
#include <lua.h>
#include <common.hpp>
#include <format>
#include <lualib.h>
#include <boost/container/flat_map.hpp>
#include <nlohmann/json.hpp>
#include <dlimport/dlimport.hpp>
#include <unordered_map>
#include <print>
#include <unordered_set>
#include <stack>
#include <queue>
#include <iostream>
using nlohmann::json;
namespace fs = std::filesystem;
using std::string_view, std::string;
namespace vws = std::views;
namespace rngs = std::ranges;
using namespace dluau;

static lua_CompileOptions copts{.debugLevel = 1};
lua_CompileOptions* dluau::compile_options{&copts};
static std::vector<Preprocessed_file> files;
static std::unordered_map<std::string, Preprocessed_file> modules;
auto dluau::get_preprocessed_modules() -> const std::unordered_map<std::string, Preprocessed_file>& {
    return modules;
}

static auto setup_state() -> std::unique_ptr<lua_State, decltype(&lua_close)> {
    auto loadstring = [](lua_State* L) -> int {
        size_t l = 0;
        const char* s = luaL_checklstring(L, 1, &l);
        const char* chunkname = luaL_optstring(L, 2, s);
        lua_setsafeenv(L, LUA_ENVIRONINDEX, false);
        size_t outsize;
        char* bc = luau_compile(s, l, dluau::compile_options, &outsize);
        string bytecode(s, outsize);
        std::free(bc);
        if (luau_load(L, chunkname, bytecode.data(), bytecode.size(), 0) == 0) {
            return 1;
        }
        lua_pushnil(L);
        lua_insert(L, -2);
        return 2;
    };
    auto collectgarbage = [](lua_State* L) -> int {
        string_view option = luaL_optstring(L, 1, "collect");
        if (option == "collect") {
            lua_gc(L, LUA_GCCOLLECT, 0);
            return 0;
        }
        if (option == "count") {
            int c = lua_gc(L, LUA_GCCOUNT, 0);
            lua_pushnumber(L, c);
            return 1;
        }
        constexpr auto errmsg{"collectgarbage must be called with 'count' or 'collect'"};
        dluau::error(L, errmsg);
    };
    auto require = [](lua_State* L) -> int {
        dluau_require(L, luaL_checkstring(L, 1));
        return 1;
    };
    auto lazyrequire = [](lua_State* L) -> int {
        dluau_lazyrequire(L, luaL_checkstring(L, 1));
        return 1;
    };
    constexpr luaL_Reg global_functions[] = {
        {"loadstring", loadstring},
        {"collectgarbage", collectgarbage},
        {"require", require},
        {"lazyrequire", lazyrequire},
        {nullptr, nullptr}
    };
    lua_State* L = luaL_newstate();
    lua_callbacks(L)->useratom = dluau::default_useratom;
    luaL_openlibs(L);
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, nullptr, global_functions);
    lua_pop(L, 1);
    return {L, lua_close}; 
}

auto dluau_run(const dluau_RunOptions* opts) -> int {
    dluau::compile_options->debugLevel = 3;
    dluau::compile_options->optimizationLevel = opts->optimization_level;
    if (opts->args) dluau::args = opts->args;
    auto state = setup_state();
    lua_State* L = state.get();

    if (opts->global_functions) {
        lua_pushvalue(L, LUA_GLOBALSINDEX);
        luaL_register(L, nullptr, opts->global_functions);
        lua_pop(L, 1);
    }
    constexpr const char* errfmt = "\033[31m{}\033[0m";
    if (opts->scripts == nullptr) {
        std::println(std::cerr, errfmt, "no sources given");
        return -1;
    }
    std::set<std::string> std_dependencies;
    std::unordered_set<std::string> processed_scripts;
    std::queue<std::string> script_queue;

    for (auto sr : vws::split(string_view(opts->scripts), dluau::arg_separator)) {
        string_view script{sr.data(), sr.size()};
        auto r = dluau::preprocess_source(script);
        if (not r) {
            std::println(std::cerr, errfmt, r.error());
            return -1;
        }
        auto& file = *r;
        for (const auto& v : file.depends_on_std) std_dependencies.emplace(v);
        script_queue.push_range(file.depends_on_scripts);
        files.push_back(std::move(file));
    }
    while (!script_queue.empty()) {
        std::string current_script = script_queue.front();
        script_queue.pop();
        if (processed_scripts.contains(common::normalize_path(current_script).string())) {
            continue;
        }
        processed_scripts.insert(current_script);
        auto r = dluau::preprocess_source(current_script);
        if (!r) {
            std::println(std::cerr, errfmt, r.error());
            return -1;
        }
        auto& file = *r;
        for (const auto& std_dep : file.depends_on_std) std_dependencies.emplace(std_dep);
        script_queue.push_range(file.depends_on_scripts);

        if (not modules.contains(file.normalized_path.string())) {
            modules.emplace(file.normalized_path.string(), std::move(file));
        }
    }
    for (const auto& path : std_dependencies) {
        std::println("STD_DEPENDENCY: {}", path);
    }
    for (const auto& [path, module] : modules) {
        std::println("SCRIPT_DEPENDENCY: {}", path);
    }
    if (not std_dependencies.empty()) {
        const fs::path bin_dir = common::get_bin_path()->parent_path();
        lua_newtable(L);
        for (const auto& dependency : std_dependencies) {
            constexpr const char* dll_fmt = "dluau_std_{}.dll";
            auto r = dlimport::init_require_module(
                L, bin_dir / std::format(dll_fmt, dependency)
            );
            if (!r) {
                std::println(std::cerr, errfmt, r.error());
                return -1;
            }
            lua_setfield(L, -2, dependency.c_str());
        }
        lua_setglobal(L, "std");
    }
    luaL_sandbox(L);
    for (const auto& pf : files) {
        if (auto result = dluau::run_file(L, pf); !result) {
            std::println(std::cerr, errfmt, result.error());
            return -1;
        }
    }
    while (dluau::tasks_in_progress()) {
        if (auto result = dluau::task_step(L); !result) {
            std::println(std::cerr, errfmt, result.error());
            return -1;
        }
    }
    std::cout << "\033[0m";
    return 0;
}
