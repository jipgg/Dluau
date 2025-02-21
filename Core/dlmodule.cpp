#include <dluau.hpp>
#include <format>
#include <common.hpp>
using std::string_view, std::string;

constexpr int unintialized{-1};
static int importfunction_sa{unintialized};

static auto index(lua_State* L) -> int {
    dluau_Dlmodule* module = dluau::to_dlmodule(L, 1);
    const string_view key = luaL_checkstring(L, 2);
    if (key == "path") {
        lua_pushstring(L, module->path.string().c_str());
        return 1;
    } else if (key == "name") {
        lua_pushstring(L, module->name.c_str());
        return 1;
    }
    luaL_argerrorL(L, 2, "index was null");
}

static auto import_function(lua_State* L) -> int {
    const string proc_key = luaL_checkstring(L, 2);
    auto opt = dluau::find_dlmodule_proc_address(*dluau::to_dlmodule(L, 1), proc_key);
    if (not opt) luaL_errorL(L, "lua_CFunction '%s' was not found ", proc_key.c_str());
    const auto fmt = std::format("dlimported:{}", proc_key);
    lua_pushcfunction(L, reinterpret_cast<lua_CFunction>(*opt), fmt.c_str());
    return 1;
}
static auto namecall(lua_State* L) -> int {
    dluau_Dlmodule& module = *dluau::to_dlmodule(L, 1);
    int atom;
    lua_namecallatom(L, &atom);
    if (atom == importfunction_sa) return import_function(L);
    dluau::error(L, "invalid namecall '{}'", atom);
}

void dluau_Dlmodule::init(lua_State* L) {
    if (luaL_newmetatable(L, dluau_Dlmodule::tname)) {
        importfunction_sa = dluau_stringatom(L, "import_fn");
        lua_setlightuserdataname(L, dluau_Dlmodule::tag, dluau_Dlmodule::tname);
        const luaL_Reg meta[] = {
            {"__index", index},
            {"__namecall", namecall},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
    }
    lua_pop(L, 1);
}
