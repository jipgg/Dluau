#include <dluau.hpp>
#include <format>
#include "luacode.h"
#include <lua.h>
#include <common.hpp>
#include <format>
#include <lualib.h>
#include <boost/container/flat_map.hpp>
#include <nlohmann/json.hpp>
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
static std::vector<Preprocessed_script> main_scripts;
static std::unordered_map<std::string, Preprocessed_script> module_scripts;
auto dluau::get_preprocessed_modules() -> const std::unordered_map<std::string, Preprocessed_script>& {
    return module_scripts;
}

static auto setup_state(const luaL_Reg* global_fns) -> std::unique_ptr<lua_State, decltype(&lua_close)> {
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
        dluau::require(L, luaL_checkstring(L, 1));
        return 1;
    };
    auto dlload = [](lua_State* L) -> int {
        auto module = dluau::dlload(L);
        if (not module) dluau::error(L, module.error());
        dluau::push_dlmodule(L, &module->get());
        return 1;
    };
    auto dlrequire = [](lua_State* L) -> int {
        const std::string name = luaL_checkstring(L, 1);
        auto result = dluau::dlload(L);
        if (!result) dluau::error(L, result.error());
        dluau_Dlmodule& module = *result;
        constexpr const char* function_signature = "dlrequire";
        auto proc = dluau::find_dlmodule_proc_address(module, function_signature);
        if (not proc) luaL_errorL(L, "module '%s' does not export a symbol '%s'.", name.c_str(), function_signature);
        lua_pushcfunction(L, reinterpret_cast<lua_CFunction>(*proc), (name + std::format(" {}", function_signature)).c_str());
        lua_call(L, 0, 1);
        return 1;
    };
    constexpr luaL_Reg default_global_fns[] = {
        {"loadstring", loadstring},
        {"collectgarbage", collectgarbage},
        {"require", require},
        {"dlload", dlload},
        {"dlrequire", dlrequire},
        {nullptr, nullptr}
    };
    lua_State* L = luaL_newstate();
    lua_callbacks(L)->useratom = dluau::default_useratom;
    luaL_openlibs(L);
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, nullptr, default_global_fns);
    if (global_fns) luaL_register(L, nullptr, global_fns);
    lua_pop(L, 1);
    dluau::open_task_library(L);
    return {L, lua_close}; 
}

static auto preprocess_dependencies(lua_State* L, string_view scripts) -> expected<void, string> {
    constexpr const char* errfmt = "\033[31m{}\033[0m";
    std::set<std::string> std_dependencies;
    std::set<std::string> dl_dependencies;
    std::unordered_set<std::string> processed_scripts;
    std::queue<std::string> module_script_queue;
    auto register_script_dependencies = [&](const Preprocessed_script& script) {
        for (const auto& std_dep : script.depends_on_std) std_dependencies.emplace(std_dep);
        for (const auto& dl_dep : script.depends_on_dls) dl_dependencies.emplace(dl_dep);
        module_script_queue.push_range(script.depends_on_scripts);
    };

    for (auto sr : vws::split(scripts, dluau::arg_separator)) {
        string_view script{sr.data(), sr.size()};
        auto r = dluau::preprocess_script(script);
        if (not r) return std::unexpected(r.error());
        auto& file = *r;
        register_script_dependencies(file);
        main_scripts.push_back(std::move(file));
    }
    while (!module_script_queue.empty()) {
        std::string current_script = module_script_queue.front();
        module_script_queue.pop();
        if (processed_scripts.contains(common::normalize_path(current_script).string())) {
            continue;
        }
        processed_scripts.insert(current_script);
        auto r = dluau::preprocess_script(current_script);
        if (!r) return std::unexpected(r.error());
        auto& file = *r;
        register_script_dependencies(file);

        if (not module_scripts.contains(file.normalized_path.string())) {
            module_scripts.emplace(file.normalized_path.string(), std::move(file));
        }
    }
    if (not std_dependencies.empty()) {
        const fs::path bin_dir = common::get_bin_path()->parent_path();
        lua_newtable(L);
        for (const auto& dependency : std_dependencies) {
            constexpr const char* dll_fmt = "dluau_std_{}.dll";
            auto r = dluau::init_require_module(
                L, bin_dir / std::format(dll_fmt, dependency)
            );
            if (!r) return std::unexpected(r.error());
            lua_setfield(L, -2, dependency.c_str());
        }
        lua_setglobal(L, "std");
    }
    if (not dl_dependencies.empty()) {
        for (const auto& dependency : dl_dependencies) {
            std::println("DL DEPENDENCIES {}", dependency);
            auto r = dluau::init_dlmodule(dependency);
            if (!r) return std::unexpected(r.error());
        }
    }
    return expected<void, string>{};
}

auto dluau_run(const dluau_RunOptions* opts) -> int {
    dluau::compile_options->debugLevel = 3;
    dluau::compile_options->optimizationLevel = opts->optimization_level;
    if (opts->args) dluau::args = opts->args;
    auto state = setup_state(opts->global_functions);
    lua_State* L = state.get();

    constexpr const char* errfmt = "\033[31m{}\033[0m";
    if (opts->scripts == nullptr) {
        std::println(std::cerr, errfmt, "no sources given");
        return -1;
    }
    if (auto r = preprocess_dependencies(L, opts->scripts); !r) {
        std::println(std::cerr, errfmt, r.error());
        return -1;
    }
    luaL_sandbox(L);
    for (const auto& pf : main_scripts) {
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
