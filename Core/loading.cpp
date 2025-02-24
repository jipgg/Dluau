#include <dluau.hpp>
#include <common.hpp>
#include <bitset>
using std::optional;
namespace rngs = std::ranges;
namespace vws = std::views;
namespace fs = std::filesystem;
using std::string_view, std::string, std::vector;
using std::expected, std::unexpected;
using boost::container::flat_set;
using boost::container::flat_map;
static flat_map<string, int> loaded_module_scripts;

auto dluau::load_file(lua_State* L, string_view path) -> expected<lua_State*, string> {
    auto result = ::dluau::preprocess_script(path);
    if (!result) return unexpected(result.error());
    return load_file(L, *result);
}
auto dluau::load_file(lua_State* L, const Preprocessed_script& script) -> expected<lua_State*, string> {
    size_t outsize;
    char* bc = luau_compile(
        script.source.data(), script.source.size(),
        compile_options, &outsize
    );
    string bytecode{bc, outsize};
    std::free(bc);
    lua_State* script_thread = lua_newthread(L);
    const int load_status = luau_load(script_thread, script.identifier.c_str(), bytecode.data(), bytecode.size(), 0);
    if (load_status == LUA_OK) {
        luaL_sandboxthread(script_thread);
        return script_thread;
    }
    return unexpected(format("failed to load '{}': {}; in source:\n{}", script.normalized_path.string(), lua_tostring(script_thread, -1), script.source));
}
auto dluau::run_file(lua_State* L, string_view script_path) -> expected<void, string> {
    auto r = ::dluau::load_file(L, script_path);
    if (not r) return unexpected(r.error());
    auto* co = *r;
    int status = lua_resume(co, L, 0);
    if (status != LUA_OK and status != LUA_YIELD) {
        return unexpected(luaL_checkstring(co, -1));
    }
    return expected<void, string>{};
}
auto dluau::run_file(lua_State* L, const Preprocessed_script& pf) -> expected<void, string> {
    auto r = ::dluau::load_file(L, pf);
    if (not r) return unexpected(r.error());
    auto* co = *r;
    int status = lua_resume(co, L, 0);
    if (status != LUA_OK and status != LUA_YIELD) {
        return unexpected(luaL_checkstring(co, -1));
    }
    return expected<void, string>{};
}
auto dluau::load_as_module_script(lua_State* L, const string& file_path, const Preprocessed_module_scripts& modules) -> expected<void, string> {
    if (not modules.contains(file_path)) {
        return unexpected("dynamic requiring is not allowed");
    }
    const string& source = modules.at(file_path).source;
    lua_State* M = lua_newthread(lua_mainthread(L));
    luaL_sandboxthread(M);
    size_t bc_len;
    char* bc_arr = luau_compile(source.data(), source.size(), ::dluau::compile_options, &bc_len);
    common::Raii free_after([&bc_arr]{std::free(bc_arr);});
    string chunkname = file_path;
    chunkname = '@' + chunkname;
    int status{-1};
    if (luau_load(M, chunkname.c_str(), bc_arr, bc_len, 0) == LUA_OK) {
        status = lua_resume(M, L, 0);
        const int top = lua_gettop(M);
        switch (status) {
            case LUA_OK:
                if (top != 1) {
                    lua_pushstring(M, "module must return 1 value.");
                    status = -1;
                }
            break;
            case LUA_YIELD:
                lua_pushstring(M, "module can not yield.");
            break;
        }
    }
    lua_xmove(M, L, 1);
    if (status != LUA_OK) {
        return unexpected(lua_tostring(L, -1));
    }
    lua_pushvalue(L, -1);
    loaded_module_scripts.emplace(file_path, lua_ref(L, -1));
    return expected<void, string>{};
}
