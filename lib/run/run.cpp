#include <goluau.h>
#include <lualib.h>
#include <core.hpp>
#include <ranges>
#include <luminutils.h>
#include <iostream>
#include <format>
#include <memory>
constexpr const char* errfmt = "\033[31mLuau: {}\033[0m\n";

int goluau_go(const goluau_GoOptions* opts) {
    goluau_compileoptions->debugLevel = opts->debug_level;
    goluau_compileoptions->optimizationLevel = opts->optimization_level;
    if (opts->args) launch_args = opts->args;
    std::unique_ptr<lua_State, decltype(&lua_close)> state{goluau_initstate(), lua_close}; 
    lua_State* L = state.get();
    main_state = L;
    if (opts->global_functions) {
        lua_pushvalue(L, LUA_GLOBALSINDEX);
        luaL_register(L, nullptr, opts->global_functions);
        lua_pop(L, 1);
    }
    if (opts->scripts == nullptr) {
        std::cerr << std::format(errfmt, "Lumin: no sources given.");
        return -1;
    }
    const char* errmsg{};
    for (auto sr : std::views::split(std::string_view(opts->scripts), arg_separator)) {
        if (auto* err = luminU_spawnscript(L, sr.data(), sr.size())) {
            errmsg = err;
        }
        std::cout << "\033[0m";
    }
    if (errmsg) {
        std::cerr << std::format(errfmt, errmsg);
        return -1;
    }
    return 0;
}
