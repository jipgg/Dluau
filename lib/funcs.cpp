#include "dluau.h"
#include <lualib.h>
#include <string>
#include <lualib.h>
#include <luacode.h>
#include <Luau/Common.h>
#include "luacode.h"
#include <iostream>
#include "Require.h"
#include <cassert>
#include <shared.hpp>

static bool codegen = true;
static int lua_loadstring(lua_State* L) {
    size_t l = 0;
    const char* s = luaL_checklstring(L, 1, &l);
    const char* chunkname = luaL_optstring(L, 2, s);
    lua_setsafeenv(L, LUA_ENVIRONINDEX, false);
    size_t outsize;
    char* bc = luau_compile(s, l, shared::compile_options, &outsize);
    std::string bytecode(s, outsize);
    std::free(bc);
    if (luau_load(L, chunkname, bytecode.data(), bytecode.size(), 0) == 0)
        return 1;
    lua_pushnil(L);
    lua_insert(L, -2); // put before error message
    return 2;          // return nil plus error message
}
static int finishrequire(lua_State* L)
{
    if (lua_isstring(L, -1))
        lua_error(L);

    return 1;
}

struct RuntimeRequireContext : public RequireResolver::RequireContext
{
    // In the context of the REPL, source is the calling context's chunkname.
    //
    // These chunknames have certain prefixes that indicate context. These
    // are used when displaying debug information (see luaO_chunkid).
    //
    // Generally, the '@' prefix is used for filepaths, and the '=' prefix is
    // used for custom chunknames, such as =stdin.
    explicit RuntimeRequireContext(std::string source)
        : source(std::move(source))
    {
    }

    std::string getPath() override
    {
        return source.substr(1);
    }

    bool isRequireAllowed() override
    {
        return isStdin() || (!source.empty() && source[0] == '@') || (!source.empty() && source[0] == '=');
    }

    bool isStdin() override
    {
        return source == "=stdin";
    }

    std::string createNewIdentifer(const std::string& path) override
    {
        return "@" + path;
    }

private:
    std::string source;
};

struct RuntimeCacheManager : public RequireResolver::CacheManager
{
    explicit RuntimeCacheManager(lua_State* L)
        : L(L)
    {
    }

    bool isCached(const std::string& path) override
    {
        luaL_findtable(L, LUA_REGISTRYINDEX, "_MODULES", 1);
        lua_getfield(L, -1, path.c_str());
        bool cached = !lua_isnil(L, -1);
        lua_pop(L, 2);

        if (cached)
            cacheKey = path;

        return cached;
    }

    std::string cacheKey;

private:
    lua_State* L;
};

struct RuntimeErrorHandler : RequireResolver::ErrorHandler
{
    explicit RuntimeErrorHandler(lua_State* L)
        : L(L)
    {
    }

    void reportError(const std::string message) override
    {
        luaL_errorL(L, "%s", message.c_str());
    }

private:
    lua_State* L;
};

int lua_require(lua_State* L)
{
    std::string name = luaL_checkstring(L, 1);

    RequireResolver::ResolvedRequire resolvedRequire;
    {
        lua_Debug ar;
        lua_getinfo(L, 1, "s", &ar);

        RuntimeRequireContext requireContext{ar.source};
        RuntimeCacheManager cacheManager{L};
        RuntimeErrorHandler errorHandler{L};

        RequireResolver resolver(std::move(name), requireContext, cacheManager, errorHandler);

        resolvedRequire = resolver.resolveRequire(
            [L, &cacheKey = cacheManager.cacheKey](const RequireResolver::ModuleStatus status)
            {
                lua_getfield(L, LUA_REGISTRYINDEX, "_MODULES");
                if (status == RequireResolver::ModuleStatus::Cached)
                    lua_getfield(L, -1, cacheKey.c_str());
            }
        );
    }

    if (resolvedRequire.status == RequireResolver::ModuleStatus::Cached)
        return finishrequire(L);

    // module needs to run in a new thread, isolated from the rest
    // note: we create ML on main thread so that it doesn't inherit environment of L
    lua_State* GL = lua_mainthread(L);
    lua_State* ML = lua_newthread(GL);
    lua_xmove(GL, L, 1);
    // new thread needs to have the globals sandboxed
    shared::script_paths.emplace(ML, resolvedRequire.absolutePath);
    luaL_sandboxthread(ML);
    // now we can compile & run module on the new thread
    size_t outsize;
    shared::process_precompiled_features(resolvedRequire.sourceCode);
    char* bytecode_data = luau_compile(resolvedRequire.sourceCode.data(),
        resolvedRequire.sourceCode.length(), shared::compile_options, &outsize);
    std::string bytecode{bytecode_data, outsize};
    std::free(bytecode_data);
    if (luau_load(ML, resolvedRequire.identifier.c_str(), bytecode.data(), bytecode.size(), 0) == 0)
    {
        int status = lua_resume(ML, L, 0);

        if (status == 0)
        {
            if (lua_gettop(ML) == 0)
                lua_pushstring(ML, "module must return a value");
            else if (!lua_istable(ML, -1) && !lua_isfunction(ML, -1))
                lua_pushstring(ML, "module must return a table or function");
        }
        else if (status == LUA_YIELD)
        {
            lua_pushstring(ML, "module can not yield");
        }
        else if (!lua_isstring(ML, -1))
        {
            lua_pushstring(ML, "unknown error while running module");
        }
    }

    // there's now a return value on top of ML; L stack: _MODULES ML
    lua_xmove(ML, L, 1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -4, resolvedRequire.absolutePath.c_str());
    // L stack: _MODULES ML result
    return finishrequire(L);
}
int lua_collectgarbage(lua_State* L) {
    const char* option = luaL_optstring(L, 1, "collect");
    if (strcmp(option, "collect") == 0) {
        lua_gc(L, LUA_GCCOLLECT, 0);
        return 0;
    }
    if (strcmp(option, "count") == 0) {
        int c = lua_gc(L, LUA_GCCOUNT, 0);
        lua_pushnumber(L, c);
        return 1;
    }
    luaL_error(L, "collectgarbage must be called with 'count' or 'collect'");
}

static int lua_scan(lua_State* L) {
    std::string in;
    std::cin >> in;
    lua_pushstring(L, in.c_str());
    return 1;
}

void dluau_loadfuncs(lua_State *L) {
    const luaL_Reg global_functions[] = {
        {"loadstring", lua_loadstring},
        {"require", lua_require},
        {"collectgarbage", lua_collectgarbage},
        {"scan", lua_scan},
        {nullptr, nullptr}
    };
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, NULL, global_functions);
    lua_pop(L, 1);
}
