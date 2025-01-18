#include <Windows.h>
#include <lualib.h>
#include <luacode.h>
#include "luminutils.h"
#include <Error_info.hpp>
#include <ranges>
#include <filesystem>
#include <lumin.h>
#include <optional>
#include <format>
#include <iostream>
#include <memory>
#include <variant>
#include <misc.hpp>
#include <functional>
#include <vector>
namespace fs = std::filesystem;
static void output_error(std::string_view msg) {
    std::cerr << std::format("\033[31m{}\033[0m\n", msg);
}

static int run(const std::string& source, std::optional<std::span<std::string_view>> args) {
    lumin_Run_options opts{
        .scripts = source.c_str(),
        .args = nullptr,
    };
    if (args) {
        static std::string argdata;
        for (auto arg : *args) {
            argdata += arg;
            argdata += ' ';
        }
        argdata.pop_back();
        opts.args = argdata.c_str();
    }
    auto err = lumin_run(opts);
    if (err) {
        std::cerr << std::format("\033[31m{}\033[0m\n", err);
        return -1;
    }
    return 0;
}
struct Project_configuration {
    std::string sources;
    int optimization_level = 0;
    int debug_level = 0;
};
static std::optional<std::string> find_config() {
    for (auto entry : fs::directory_iterator(fs::current_path())) {
        const std::string str = entry.path().string();
        if (not str.ends_with(".proj.luau")) continue;
        return str;
    }
    return std::nullopt;
}
struct Defer {
    std::function<void()> f;
    Defer(std::function<void()> f): f(f) {}
    ~Defer() {f();}
};
static std::variant<Project_configuration, Error_info> read_config(const std::string& path) {
    std::unique_ptr<lua_State, decltype(&lua_close)> state{luaL_newstate(), lua_close};
    lua_State* L = state.get();
    auto source = misc::read_file(path);
    if (not source) return Error_info{"couldn't read source file"};
    size_t bytecode_size;
    char* bytecode = luau_compile(source->data(), source->size(), nullptr, &bytecode_size);
    Defer free_bytecode([&bytecode] {std::free(bytecode);});
    if (luau_load(L, "Project configuration file", bytecode, bytecode_size, 0) != LUA_OK) {
        return Error_info{std::string(bytecode, bytecode_size)};
    }
    if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
        return Error_info{lua_tostring(L, -1)};
    }
    Project_configuration config{};
    lua_getfield(L, -1, "sources");
    if (lua_isnil(L, -1)) return Error_info{"no sources given."};
    if (lua_isstring(L, -1)) {
        config.sources = lua_tostring(L, -1);
    } else if (lua_istable(L, -1)) {
        const int len = lua_objlen(L, -1);
        for (int i{1}; i <= len; ++i) {
            lua_rawgeti(L, -1, i);
            if (not lua_isstring(L, -1)) return Error_info{"is not a string"};
            config.sources += lua_tostring(L, -1);
            config.sources += ' ';
            lua_pop(L, 1);
        }
        config.sources.pop_back();
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "optimization_level");
    if (lua_isnil(L, -1)) config.optimization_level = 0;
    else config.optimization_level = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, -1, "debug_level");
    if (lua_isnil(L, -1)) config.debug_level= 1;
    else config.debug_level = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    return config;
}
int lumin_main(std::span<std::string_view> args) {
    std::optional<std::span<std::string_view>> run_args;
    auto it = std::ranges::find(args, "--");
    if (it != args.end() and it != args.end() - 1) {
        run_args = args.subspan(++it - args.begin());
    }
    if (args[0] == "run") {
        //if (args.size() > 1) return run(std::string(args[1]), run_args);
        auto found_config_path = find_config();
        if (not found_config_path) {
            output_error("couldn't find configuration file");
            return -1;
        }
        auto config_var = read_config(*found_config_path);
        if (auto* err = std::get_if<Error_info>(&config_var)) {
            output_error(err->message());
            return -1;
        }
        const auto& config = std::get<Project_configuration>(config_var);
        return run(config.sources, run_args);
    }
    return 0;
}
