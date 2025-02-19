#include <dluau.hpp>
#include <common.hpp>
#include <bitset>
using std::optional;
namespace rngs = std::ranges;
namespace vws = std::views;
using std::string_view, std::string, std::vector;
using std::expected, std::unexpected;
using boost::container::flat_set;

namespace dluau {
auto load_file(lua_State* L, string_view path) -> expected<lua_State*, string> {
    auto result = preprocess_source(path);
    if (!result) return unexpected(result.error());
    return load_file(L, *result);
}
auto load_file(lua_State* L, const Preprocessed_file& pf) -> expected<lua_State*, string> {
    const auto& [full_path, identifier, source] = pf;
    size_t outsize;
    char* bc = luau_compile(
        source.data(), source.size(),
        compile_options, &outsize
    );
    string bytecode{bc, outsize};
    std::free(bc);
    lua_State* script_thread = lua_newthread(L);
    detail::get_script_paths().emplace(script_thread, full_path.string());
    const int load_status = luau_load(script_thread, identifier.c_str(), bytecode.data(), bytecode.size(), 0);
    if (load_status == LUA_OK) {
        luaL_sandboxthread(script_thread);
        return script_thread;
    }
    return unexpected(format("failed to load '{}'\nreason: {}\nsource: {}", full_path.string(), lua_tostring(script_thread, -1), source));
}
auto has_permissions(lua_State* L) -> bool {
    lua_Debug ar;
    if (not lua_getinfo(L, 1, "s", &ar)) return false;
    if (ar.source[0] == '@' or ar.source[0] == '=') return true;
    return false;
}
auto run_file(lua_State* L, string_view script_path) -> expected<void, string> {
    auto r = ::dluau::load_file(L, script_path);
    if (not r) return unexpected(r.error());
    auto* co = *r;
    int status = lua_resume(co, L, 0);
    if (status != LUA_OK and status != LUA_YIELD) {
        return unexpected(luaL_checkstring(co, -1));
    }
    return expected<void, string>{};
}
auto run_file(lua_State* L, const Preprocessed_file& pf) -> expected<void, string> {
    auto r = ::dluau::load_file(L, pf);
    if (not r) return unexpected(r.error());
    auto* co = *r;
    int status = lua_resume(co, L, 0);
    if (status != LUA_OK and status != LUA_YIELD) {
        return unexpected(luaL_checkstring(co, -1));
    }
    return expected<void, string>{};
}
}
