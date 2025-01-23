#include <luauxt.h>
#include <lualib.h>
#include <core.hpp>
#include <ranges>
#include <iostream>
#include <format>
#include <memory>
constexpr const char* errfmt = "\033[31mLuau: {}\033[0m\n";

int luauxt_run(const goluau_GoOptions* opts) {
    luauxt_compileoptions->debugLevel = opts->debug_level;
    luauxt_compileoptions->optimizationLevel = opts->optimization_level;
    if (opts->args) launch_args = opts->args;
    std::unique_ptr<lua_State, decltype(&lua_close)> state{luauxt_newstate(), lua_close}; 
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
    std::string errmsg;
    for (auto sr : std::views::split(std::string_view(opts->scripts), arg_separator)) {
        std::string_view script{sr.data(), sr.size()};
        if (auto err = script_utils::run(L, script)) {
            errmsg = err->message();
        }
        std::cout << "\033[0m";
    }
    if (not errmsg.empty()) {
        std::cerr << std::format(errfmt, errmsg);
        return -1;
    }
    return 0;
}
