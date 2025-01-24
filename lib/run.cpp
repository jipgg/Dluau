#include <dluau.h>
#include <lualib.h>
#include <shared.hpp>
#include <ranges>
#include <iostream>
#include <format>
#include <filesystem>
#include <memory>
constexpr const char* errfmt = "\033[31mLuau: {}\033[0m\n";
constexpr const char* errfmt_exception = "\033[31mCaught exception: {}\033[0m\n";

void dluau_openlibs(lua_State *L) {
    luaL_openlibs(L);
    dluau_loadfuncs(L);
    dluauopen_dlimport(L);
}
int dluau_newuserdatatag() {
    static int curr_type_tag = 1;
    return curr_type_tag++; 
}
int dluau_newlightuserdatatag() {
    static int curr_type_tag = 1;
    return curr_type_tag++; 
}

lua_State* dluau_newstate() {
    lua_State* L = luaL_newstate();
    lua_callbacks(L)->useratom = shared::default_useratom;
    dluau_openlibs(L);
    shared::push_metadatatable(L);
    lua_setglobal(L, "meta");
    return L;
}

int dluau_run(const dluau_RunOptions* opts) {
    shared::compile_options->debugLevel = opts->debug_level;
    shared::compile_options->optimizationLevel = opts->optimization_level;
    if (opts->args) shared::args = opts->args;
    std::unique_ptr<lua_State, decltype(&lua_close)> state{dluau_newstate(), lua_close}; 
    lua_State* L = state.get();
    shared::main_state = L;
    if (opts->global_functions) {
        lua_pushvalue(L, LUA_GLOBALSINDEX);
        luaL_register(L, nullptr, opts->global_functions);
        lua_pop(L, 1);
    }
    luaL_sandbox(L);
    if (opts->scripts == nullptr) {
        std::cerr << std::format(errfmt, "Lumin: no sources given.");
        return -1;
    }
    std::string errmsg;
    for (auto sr : std::views::split(std::string_view(opts->scripts), arg_separator)) {
        std::string_view script{sr.data(), sr.size()};
        if (auto err = shared::script_utils::run(L, script)) {
            errmsg = err->message();
            break;
        }
        std::cout << "\033[0m";
    }
    if (not errmsg.empty()) {
        std::cerr << std::format(errfmt, errmsg);
        return -1;
    }
    return 0;
}
