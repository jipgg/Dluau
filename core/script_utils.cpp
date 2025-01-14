#include "luminutils.h"
#include "lumin.h"
#include <fstream>
#include <optional>
#include <format>
#include "luacode.h"
#include <lua.h>
#include <filesystem>
#include <iostream>
#include <format>
#include <unordered_map>
#include <lualib.h>
namespace fs = std::filesystem;
static lua_CompileOptions _lumin_compileoptions{};
lua_CompileOptions* lumin_globalcompileoptions{&_lumin_compileoptions};

struct Script {
    lua_State* thread;
    std::string path;
};
static std::optional<std::string> read_file(const fs::path &path) {
    if (not fs::exists(path)) [[unlikely]] {
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
lua_State* luminU_loadscript(lua_State* L, const char* path) {
    const fs::path script_path{path};
    std::optional<std::string> source = read_file(script_path);
    using namespace std::string_literals;
    if (not source) return nullptr;
    auto identifier = script_path.filename().string();
    identifier = "=" + identifier;
    size_t outsize;
    char* bc = luau_compile(source->data(), source->size(),
                            lumin_globalcompileoptions, &outsize);
    std::string bytecode{bc, outsize};
    std::free(bc);
    lua_State* script_thread = lua_newthread(L);
    const int load_status = luau_load(script_thread, identifier.c_str(), bytecode.data(), bytecode.size(), 0);
    if (load_status == LUA_OK) {
        luaL_sandboxthread(script_thread);
        return script_thread;
    }
    return nullptr;
}
void luminU_spawnscript(lua_State* L, const char* script_path) {
    auto r = luminU_loadscript(L, script_path);
    if (not r) luaL_errorL(L, "failed to load script '%s'", script_path);
    int status = lua_resume(r, L, 0);
    if (status != LUA_OK) {
        std::cerr << std::format("\033[31m{}\033[0m\n", luaL_checkstring(r, -1));
    }
}
