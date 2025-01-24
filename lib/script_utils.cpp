#include "dluau.h"
#include <fstream>
#include <optional>
#include <format>
#include "luacode.h"
#include <lua.h>
#include <filesystem>
#include <ErrorInfo.hpp>
#include <iostream>
#include <format>
#include <unordered_map>
#include <lualib.h>
#include <boost/container/flat_map.hpp>
#include <shared.hpp>
namespace fs = std::filesystem;
static lua_CompileOptions copts{.debugLevel = 1};
lua_CompileOptions* shared::compile_options{&copts};

static std::optional<std::string> read_file(const fs::path &path) {
    if (not fs::exists(path)) {
        return std::nullopt;
    }
    std::ifstream file_in{};
    file_in.open(path.c_str());
    if  (not file_in.is_open()) [[unlikely]] {
        return std::nullopt;
    }
    std::string curr_line{};
    std::stringstream file_stream{};
    while (std::getline(file_in,curr_line)) {
        file_stream << curr_line << '\n';
    }
    return file_stream.str();
}
namespace shared::script_utils {
std::optional<lua_State*> load(lua_State* L, std::string_view path) {
    const fs::path script_path{path};
    std::optional<std::string> source = read_file(script_path);
    using namespace std::string_literals;
    if (not source) return std::nullopt;
    auto identifier = script_path.filename().string();
    identifier = "=" + identifier;
    size_t outsize;
    char* bc = luau_compile(
        source->data(), source->size(),
        shared::compile_options, &outsize
    );
    std::string bytecode{bc, outsize};
    std::free(bc);
    lua_State* script_thread = lua_newthread(L);
    script_paths.emplace(script_thread, fs::absolute(path).string());
    const int load_status = luau_load(script_thread, identifier.c_str(), bytecode.data(), bytecode.size(), 0);
    if (load_status == LUA_OK) {
        luaL_sandboxthread(script_thread);
        return script_thread;
    }
    return std::nullopt;
}
std::optional<ErrorInfo> run(lua_State* L, std::string_view script_path) {
    auto r = load(L, script_path);
    if (not r) return ErrorInfo(std::format("failed to load script '{}'", script_path));
    int status = lua_resume(*r, L, 0);
    if (status != LUA_OK and status != LUA_YIELD) {
        return ErrorInfo{luaL_checkstring(*r, -1)};
    }
    return std::nullopt;
}
}
