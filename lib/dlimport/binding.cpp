#include "lib.hpp"
using std::optional, std::string_view;

enum class param_type {
    int_v, void_v, float_v, string_v, double_v, long_v, short_v, longlong_v, pointer_v
};
struct param_binding {
    std::vector<param_type> types;
    param_type get_return_type() const {return types[0];}
    uintptr_t function_pointer;
};
static int call_binding(lua_State* L) {
    const param_binding& binding = *static_cast<param_binding*>(lua_touserdata(L, lua_upvalueindex(1)));
    DCCallVM* vm = glob::call_vm.get();
    dcReset(vm);
    using pt = param_type;
    for (int i{1}; i < binding.types.size(); ++i) {
        switch(binding.types.at(i)) {
            case pt::int_v:
                dcArgInt(vm, luaL_checkinteger(L, i));
                break;
            case pt::double_v:
                dcArgDouble(vm, luaL_checknumber(L, i));
                break;
            case pt::float_v:
                dcArgFloat(vm, static_cast<float>(luaL_checknumber(L, i)));
                break;
            case pt::string_v:
                dcArgPointer(vm, (DCpointer)luaL_checkstring(L, i));
                break;
            case pt::short_v:
                dcArgShort(vm, luaL_checkinteger(L, i));
                break;
            case pt::long_v:
                dcArgLong(vm, luaL_checkinteger(L, i));
                break;
            case pt::longlong_v:
                dcArgLongLong(vm, luaL_checkinteger(L, i));
                break;
            case pt::pointer_v:
                if (not lua_islightuserdata(L, i)) luaL_typeerrorL(L, i, "not light userdata.");
                dcArgPointer(vm, lua_tolightuserdata(L, i));
            case pt::void_v:
                break;
        }
    }
    DCpointer fnptr = reinterpret_cast<DCpointer>(binding.function_pointer);
    switch(binding.types.at(0)) {
        case pt::int_v:
            lua_pushinteger(L, dcCallInt(vm, fnptr));
            return 1;
        case pt::double_v:
            lua_pushnumber(L, dcCallDouble(vm, fnptr));
            return 1;
        case pt::float_v:
            lua_pushnumber(L, dcCallFloat(vm, fnptr));
            return 1;
        case pt::string_v:
            lua_pushstring(L, static_cast<const char*>(dcCallPointer(vm, fnptr)));
            return 1;
        case pt::short_v:
            lua_pushinteger(L, dcCallShort(vm, fnptr));
            return 1;
        case pt::long_v:
            lua_pushinteger(L, dcCallLong(vm, fnptr));
            return 1;
        case pt::longlong_v:
            lua_pushinteger(L, dcCallLongLong(vm, fnptr));
            return 1;
        case pt::pointer_v:
            lua_pushlightuserdata(L, dcCallPointer(vm, fnptr));
            return 1;
        case pt::void_v:
            dcCallVoid(vm, fnptr);
            return 0;
    }
    luaL_errorL(L, "call error");
}
static optional<param_type> string_to_param_type(string_view str) {
    using pt = param_type;
    if (str == "int") {
        return pt::int_v;
    } else if (str == "double") {
        return pt::double_v;
    } else if (str == "void") {
        return pt::void_v;
    } else if (str == "string") {
        return pt::string_v;
    } else if (str == "float") {
        return pt::float_v;
    } else if (str == "short") {
        return pt::short_v;
    } else if (str == "long") {
        return pt::long_v;
    } else if (str == "long long") {
        return pt::longlong_v;
    } else if (str == "pointer") {
        return pt::pointer_v;
    }
    return std::nullopt;
}
int dlmodule::create_binding(lua_State* L) {
    dlmodule* module = util::lua_tomodule(L, 1);
    param_binding binding{};
    const int top = lua_gettop(L);
    string_view return_type = luaL_checkstring(L, 2);
    auto res = string_to_param_type(return_type);
    if (not res) luaL_argerrorL(L, 2, "not a c type");
    binding.types.push_back(std::move(*res));

    auto proc = util::find_proc_address(*module, luaL_checkstring(L, 3));
    if (not proc) luaL_argerrorL(L, 3, "couldn't find address");
    binding.function_pointer = *proc;

    if (top > 3) {
        for (int i{4}; i <= top; ++i) {
            auto res = string_to_param_type(luaL_checkstring(L, i));
            if (not res) luaL_argerrorL(L, i, "not a c type");
            else if (*res == param_type::void_v) luaL_argerrorL(L, i, "an argument type cannot be void.");
            binding.types.push_back(std::move(*res));
        }
    }
    param_binding* up = static_cast<param_binding*>(lua_newuserdatadtor(L, sizeof(param_binding), [](void* ud) {
        static_cast<param_binding*>(ud)->~param_binding();
    }));
    new (up) param_binding{std::move(binding)};
    lua_pushcclosure(L, call_binding, "call_binding", 1);
    return 1;
}
