#include "common.hpp"
#include <fstream>
#include "Error.hpp"
#include <optional>
#include <format>
#include "luacode.h"
#include "lua_base.hpp"
#include <filesystem>
#include <lualib.h>
namespace fs = std::filesystem;
std::variant<lua_State*, Error> load_script(lua_State* L, const fs::path& script_path) noexcept {
    std::optional<std::string> source = read_file(script_path);
    using namespace std::string_literals;
    if (not source) return Error(std::format("Couldn't read source '{}'.", script_path.string())); 
    auto identifier = script_path.filename().string();
    identifier = "=" + identifier;
    size_t outsize;
    char* bc = luau_compile(source->data(), source->size(), compile_options(), &outsize);
    std::string bytecode{bc, outsize};
    std::free(bc);
    lua_State* script_thread = lua_newthread(L);
    const int load_status = luau_load(script_thread, identifier.c_str(), bytecode.data(), bytecode.size(), 0);
    if (load_status == LUA_OK) {
        luaL_sandboxthread(script_thread);
        return script_thread;
    }
    return Error(std::format("failed to load source: {}", bytecode));
}
std::optional<Error> spawn_script(lua_State* L, const fs::path& script_path) noexcept {
    auto ret = load_script(L, script_path);
    if (Error* err = std::get_if<Error>(&ret)) {
        return err->propagate();
    }
    lua_State* co = std::get<lua_State*>(ret);
    int status = lua_resume(co, L, 0);
    if (status != LUA_OK) {
        return Error(luaL_checkstring(co, -1));
    }
    return std::nullopt;
} 
std::optional<std::string> read_file(const fs::path &path) {
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
