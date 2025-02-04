#include "dlimport.hpp"
using std::optional, std::string_view;
using dlimport::dlmodule;
using dlimport::c_type;

static std::unique_ptr<DCCallVM, decltype(&dcFree)> call_vm{dcNewCallVM(1024), dcFree};

struct c_function_binding {
    std::vector<c_type> types;
    c_type get_return_type() const {return types[0];}
    uintptr_t function_pointer;
};
static int call_binding(lua_State* L) {
    const auto& binding = *static_cast<c_function_binding*>(lua_touserdata(L, lua_upvalueindex(1)));
    DCCallVM* vm = call_vm.get();
    dcReset(vm);
    using ct = c_type;
    for (int i{1}; i < binding.types.size(); ++i) {
        switch(binding.types.at(i)) {
            case ct::c_bool:
                dcArgBool(vm, luaL_checkboolean(L, i));
                break;
            case ct::c_int:
                dcArgInt(vm, luaL_checkinteger(L, i));
                break;
            case ct::c_uint:
                dcArgInt(vm, (unsigned int)luaL_checkinteger(L, i));
                break;
            case ct::c_short:
            case ct::c_ushort:
                dcArgShort(vm, luaL_checkinteger(L, i));
                break;
            case ct::c_char:
            case ct::c_uchar:
                if (lua_isstring(L, i)) dcArgChar(vm, *lua_tostring(L, i));
                else if (lua_isnumber(L, i)) dcArgChar(vm, lua_tointeger(L, i));
                break;
            case ct::c_long:
            case ct::c_ulong:
                dcArgLong(vm, luaL_checkinteger(L, i));
                break;
            case ct::c_double:
                dcArgDouble(vm, luaL_checknumber(L, i));
                break;
            case ct::c_float:
                dcArgFloat(vm, static_cast<float>(luaL_checknumber(L, i)));
                break;
            case ct::c_char_ptr:
                dcArgPointer(vm, (DCpointer)luaL_checkstring(L, i));
                break;
            case ct::c_void_ptr:
                if (not lua_islightuserdata(L, i)) luaL_typeerrorL(L, i, "not light userdata.");
                dcArgPointer(vm, lua_tolightuserdata(L, i));
            case ct::c_void:
                break;
        }
    }
    DCpointer fnptr = reinterpret_cast<DCpointer>(binding.function_pointer);
    switch(binding.types.at(0)) {
        case ct::c_bool:
            lua_pushboolean(L, dcCallBool(vm, fnptr));
            return 1;
        case ct::c_int:
            lua_pushinteger(L, dcCallInt(vm, fnptr));
            return 1;
        case ct::c_uint:
            lua_pushinteger(L, static_cast<unsigned int>(dcCallInt(vm, fnptr)));
            return 1;
        case ct::c_short:
            lua_pushinteger(L, dcCallShort(vm, fnptr));
            return 1;
        case ct::c_ushort:
            lua_pushinteger(L, (unsigned short)dcCallShort(vm, fnptr));
            return 1;
        case ct::c_long:
            lua_pushinteger(L, dcCallLong(vm, fnptr));
            return 1;
        case ct::c_ulong:
            lua_pushinteger(L, (unsigned long)dcCallLong(vm, fnptr));
            return 1;
        case ct::c_char:
            lua_pushinteger(L, dcCallChar(vm, fnptr));
            return 1;
        case ct::c_uchar:
            lua_pushinteger(L, (unsigned char)dcCallChar(vm, fnptr));
            return 1;
        case ct::c_double:
            lua_pushnumber(L, dcCallDouble(vm, fnptr));
            return 1;
        case ct::c_float:
            lua_pushnumber(L, dcCallFloat(vm, fnptr));
            return 1;
        case ct::c_char_ptr:
            lua_pushstring(L, static_cast<char*>(dcCallPointer(vm, fnptr)));
            return 1;
        case ct::c_void_ptr:
            lua_pushlightuserdata(L, dcCallPointer(vm, fnptr));
            return 1;
        case ct::c_void:
            dcCallVoid(vm, fnptr);
            return 0;
    }
    luaL_errorL(L, "call error");
}
optional<c_type> dlimport::string_to_param_type(string_view str) {
    using ct = c_type;
    static const boost::container::flat_map<string_view, c_type> map {
        {"int", ct::c_int},
        {"uint", ct::c_uint},
        {"short", ct::c_short},
        {"ushort", ct::c_ushort},
        {"long", ct::c_short},
        {"ulong", ct::c_ulong},
        {"char", ct::c_char},
        {"uchar", ct::c_uchar},
        {"bool", ct::c_bool},
        {"void*", ct::c_void_ptr},
        {"void", ct::c_void},
        {"char*", ct::c_char_ptr},
    };
    if (not map.contains(str)) return std::nullopt;
    return map.at(str);
}
int dlmodule::create_binding(lua_State* L) {
    dlmodule* module = dlimport::lua_tomodule(L, 1);
    c_function_binding binding{};
    const int top = lua_gettop(L);
    string_view return_type = luaL_checkstring(L, 2);
    auto res = string_to_param_type(return_type);
    if (not res) luaL_argerrorL(L, 2, "not a c type");
    binding.types.push_back(std::move(*res));

    auto proc = dlimport::find_proc_address(*module, luaL_checkstring(L, 3));
    if (not proc) luaL_argerrorL(L, 3, "couldn't find address");
    binding.function_pointer = *proc;

    if (top > 3) {
        for (int i{4}; i <= top; ++i) {
            auto res = string_to_param_type(luaL_checkstring(L, i));
            if (not res) luaL_argerrorL(L, i, "not a c type");
            else if (*res == c_type::c_void) luaL_argerrorL(L, i, "an argument type cannot be void.");
            binding.types.push_back(std::move(*res));
        }
    }
    c_function_binding* up = static_cast<c_function_binding*>(lua_newuserdatadtor(L, sizeof(c_function_binding), [](void* ud) {
        static_cast<c_function_binding*>(ud)->~c_function_binding();
    }));
    new (up) c_function_binding{std::move(binding)};
    lua_pushcclosure(L, call_binding, "c_function_binding", 1);
    return 1;
}
