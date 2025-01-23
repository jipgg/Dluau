#include "lib.hpp"
#include <format>
#include <iostream>

enum class ParamType {
    Int, Void, Float, String, Double
};
struct Binding {
    std::vector<ParamType> types;
    ParamType get_return_type() const {return types[0];}
    uintptr_t function_pointer;
};
static int call_binding(lua_State* L) {
    const Binding& binding = *static_cast<Binding*>(lua_touserdata(L, lua_upvalueindex(1)));
    DCCallVM* vm = glob::call_vm.get();
    dcReset(vm);
    using Pt = ParamType;
    for (int i{1}; i < binding.types.size(); ++i) {
        switch(binding.types.at(i)) {
            case Pt::Int:
                dcArgInt(vm, luaL_checkinteger(L, i));
                break;
            case Pt::Double:
                dcArgDouble(vm, luaL_checknumber(L, i));
                break;
            case Pt::Float:
                dcArgFloat(vm, static_cast<float>(luaL_checknumber(L, i)));
                break;
            case Pt::String:
                dcArgPointer(vm, (DCpointer)luaL_checkstring(L, i));
                break;
            case Pt::Void:
                break;
        }
    }
    DCpointer fnptr = reinterpret_cast<DCpointer>(binding.function_pointer);
    switch(binding.types.at(0)) {
        case Pt::Int:
            lua_pushinteger(L, dcCallInt(vm, fnptr));
            return 1;
        case Pt::Double:
            lua_pushnumber(L, dcCallDouble(vm, fnptr));
            return 1;
        case Pt::Float:
            lua_pushnumber(L, dcCallFloat(vm, fnptr));
            return 1;
        case Pt::String:
            lua_pushstring(L, static_cast<const char*>(dcCallPointer(vm, fnptr)));
            return 1;
        case Pt::Void:
            dcCallVoid(vm, fnptr);
            return 0;
    }
    luaL_errorL(L, "call error");
}
static Opt<ParamType> string_to_param_type(StrView str) {
    using Pt = ParamType;
    if (str == "int") {
        return Pt::Int;
    } else if (str == "double") {
        return Pt::Double;
    } else if (str == "void") {
        return Pt::Void;
    } else if (str == "string") {
        return Pt::String;
    } else if (str == "float") {
        return Pt::Float;
    }
    return std::nullopt;
}
int Dlmodule::create_binding(lua_State* L) {
    Dlmodule* module = util::lua_tomodule(L, 1);
    Binding binding{};
    const int top = lua_gettop(L);
    StrView return_type = luaL_checkstring(L, 2);
    using Pt = ParamType;
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
            binding.types.push_back(std::move(*res));
        }
    }
    Binding* up = static_cast<Binding*>(lua_newuserdatadtor(L, sizeof(Binding), [](void* ud) {
        static_cast<Binding*>(ud)->~Binding();
    }));
    new (up) Binding{std::move(binding)};
    std::cout << std::format("s{} fn{}\n", up->types.size(), up->function_pointer);
    lua_pushcclosure(L, call_binding, "call_binding", 1);
    return 1;
}
