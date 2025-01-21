#pragma once
#include <string>
#include <optional>
#include <span>
#include <string_view>
#include <lumin.h>
#include <filesystem>
#include <functional>
#include <lualib.h>
#include <Error_info.hpp>
#include <variant>
#include <misc.hpp>
namespace fs = std::filesystem;

struct Project_configuration {
    std::string sources;
    int optimization_level = 0;
    int debug_level = 0;
    std::string console{"none"};
};
static std::optional<std::string> find_config(const std::string& name = "") {
    for (auto entry : fs::recursive_directory_iterator(fs::current_path())) {
        const std::string str = entry.path().string();
        if (not str.ends_with(name + ".proj.luau")) continue;
        return str;
    }
    return std::nullopt;
}
struct Defer {
    std::function<void()> f;
    Defer(std::function<void()> f): f(f) {}
    ~Defer() {f();}
};
static std::variant<Project_configuration, ErrorInfo> read_config(const std::string& path) {
    std::unique_ptr<lua_State, decltype(&lua_close)> state{luaL_newstate(), lua_close};
    lua_State* L = state.get();
    auto source = misc::read_file(path);
    if (not source) return ErrorInfo{"couldn't read source file"};
    size_t bytecode_size;
    char* bytecode = luau_compile(source->data(), source->size(), nullptr, &bytecode_size);
    Defer free_bytecode([&bytecode] {std::free(bytecode);});
    if (luau_load(L, "Project configuration file", bytecode, bytecode_size, 0) != LUA_OK) {
        return ErrorInfo{std::string(bytecode, bytecode_size)};
    }
    if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
        return ErrorInfo{lua_tostring(L, -1)};
    }
    Project_configuration config{};
    lua_getfield(L, -1, "sources");
    if (lua_isnil(L, -1)) return ErrorInfo{"no sources given."};
    if (lua_isstring(L, -1)) {
        config.sources = lua_tostring(L, -1);
    } else if (lua_istable(L, -1)) {
        const int len = lua_objlen(L, -1);
        for (int i{1}; i <= len; ++i) {
            lua_rawgeti(L, -1, i);
            if (not lua_isstring(L, -1)) return ErrorInfo{"is not a string"};
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
    lua_getfield(L, -1, "console");
    if (lua_isstring(L, -1)) config.console = lua_tostring(L, -1);
    lua_pop(L, 1);
    return config;
}
