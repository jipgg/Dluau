#include <dluau.hpp>
#include <common.hpp>
using std::optional;
namespace rngs = std::ranges;

namespace dluau {
auto load_file(lua_State* L, string_view path) -> expected<lua_State*, string> {
    string script_path{path};
    optional<string> source = common::read_file(script_path);
    if (not source) return unexpected(format("couldn't read source '{}'.", script_path));
    auto normalized = common::normalize_path(script_path);
    dluau::precompile(*source, get_precompiled_library_values(normalized.string()));
    string identifier{fs::relative(script_path).string()};
    rngs::replace(identifier, '\\', '/');
    identifier = '=' + identifier;
    size_t outsize;
    char* bc = luau_compile(
        source->data(), source->size(),
        compile_options, &outsize
    );
    string bytecode{bc, outsize};
    std::free(bc);
    lua_State* script_thread = lua_newthread(L);
    detail::get_script_paths().emplace(script_thread, normalized.string());
    const int load_status = luau_load(script_thread, identifier.c_str(), bytecode.data(), bytecode.size(), 0);
    if (load_status == LUA_OK) {
        luaL_sandboxthread(script_thread);
        return script_thread;
    }
    return unexpected(format("failed to load '{}'\nreason: {}\nsource: {}", script_path, lua_tostring(script_thread, -1), *source));
}
}
