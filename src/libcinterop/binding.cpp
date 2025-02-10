#include "cinterop.hpp"

static Unique<DCCallVM, decltype(&dcFree)> call_vm{dcNewCallVM(1024), dcFree};

struct C_func_binding {
    Vector<Variant<C_type, Shared<Struct_info>>> types;
    Variant<C_type, Shared<Struct_info>> get_return_type() const {return types[0];}
    uintptr_t function_pointer;
};
static int call_binding(lua_State* L) {
    const auto& binding = *static_cast<C_func_binding*>(lua_touserdata(L, lua_upvalueindex(1)));
    DCCallVM* vm = call_vm.get();
    dcReset(vm);
    using ct = C_type;
    int curr_aggr{};
    for (int i{1}; i < binding.types.size(); ++i) {
        if (auto* type = std::get_if<C_type>(&binding.types[i])) {
            switch(*type) {
                case ct::c_bool:
                    dcArgBool(vm, luaL_checkboolean(L, i));
                    break;
                case ct::c_int:
                    dcArgInt(vm, dluauC_checkint(L, i));
                    break;
                case ct::c_uint:
                    dcArgInt(vm, dluauC_checkuint(L, i));
                    break;
                case ct::c_short:
                    dcArgShort(vm, dluauC_checkshort(L, i));
                    break;
                case ct::c_ushort:
                    dcArgShort(vm, dluauC_checkushort(L, i));
                    break;
                case ct::c_char:
                    dcArgChar(vm, dluauC_checkchar(L, i));
                    break;
                case ct::c_uchar:
                    dcArgChar(vm, dluauC_checkuchar(L, i));
                    break;
                case ct::c_long:
                    dcArgLong(vm, dluauC_checklong(L, i));
                    break;
                case ct::c_ulong:
                    dcArgLong(vm, dluauC_checkulong(L, i));
                    break;
                case ct::c_float:
                    dcArgFloat(vm, dluauC_checkfloat(L, i));
                    break;
                case ct::c_double:
                    dcArgDouble(vm, luaL_checknumber(L, i));
                    break;
                case ct::c_string:
                    dcArgPointer(vm, (DCpointer)luaL_checkstring(L, i));
                    break;
                case ct::c_void_ptr:
                    if (not lua_islightuserdata(L, i)) luaL_typeerrorL(L, i, "not light userdata.");
                    dcArgPointer(vm, lua_tolightuserdata(L, i));
                    break;
                case ct::c_void:
                    break;
            }
        } else {
            auto& si = std::get<Shared<Struct_info>>(binding.types[i]);
            void* data = lua_touserdata(L, i);
            dcArgAggr(vm, si->aggr.get(), data);
        }
    }
    DCpointer fnptr = reinterpret_cast<DCpointer>(binding.function_pointer);
    if (auto* type = std::get_if<C_type>(&binding.types.at(0))) {
        switch(*type) {
            case ct::c_bool:
                lua_pushboolean(L, dcCallBool(vm, fnptr));
                return 1;
            case ct::c_int:
                dluauC_pushint(L, dcCallInt(vm, fnptr));
                return 1;
            case ct::c_uint:
                dluauC_pushuint(L, static_cast<unsigned int>(dcCallInt(vm, fnptr)));
                return 1;
            case ct::c_short:
                dluauC_pushshort(L, dcCallShort(vm, fnptr));
                return 1;
            case ct::c_ushort:
                dluauC_pushushort(L, (unsigned short)dcCallShort(vm, fnptr));
                return 1;
            case ct::c_long:
                dluauC_pushlong(L, dcCallLong(vm, fnptr));
                return 1;
            case ct::c_ulong:
                dluauC_pushulong(L, (unsigned long)dcCallLong(vm, fnptr));
                return 1;
            case ct::c_char:
                dluauC_pushchar(L, dcCallChar(vm, fnptr));
                return 1;
            case ct::c_uchar:
                dluauC_pushuchar(L, (unsigned char)dcCallChar(vm, fnptr));
                return 1;
            case ct::c_double:
                lua_pushnumber(L, dcCallDouble(vm, fnptr));
                return 1;
            case ct::c_float:
                dluauC_pushfloat(L, dcCallFloat(vm, fnptr));
                return 1;
            case ct::c_string:
                lua_pushstring(L, static_cast<char*>(dcCallPointer(vm, fnptr)));
                return 1;
            case ct::c_void_ptr:
                lua_pushlightuserdata(L, dcCallPointer(vm, fnptr));
                return 1;
            case ct::c_void:
                dcCallVoid(vm, fnptr);
                return 0;
        }
    } else {
        auto& si = std::get<Shared<Struct_info>>(binding.types[0]);
        //void* buf = lua_newbuffer(L, si->memory_size);
        void* instance = si->newinstance(L);
        dcCallAggr(vm, fnptr, si->aggr.get(), instance);
        return 1;
    }
    luaL_errorL(L, "call error");
}
Opt<C_type> string_to_param_type(Strview str) {
    using ct = C_type;
    static const Flat_map<Strview, C_type> map {
        {"c_int", ct::c_int},
        {"c_uint", ct::c_uint},
        {"c_short", ct::c_short},
        {"c_ushort", ct::c_ushort},
        {"c_long", ct::c_long},
        {"c_ulong", ct::c_ulong},
        {"c_char", ct::c_char},
        {"c_uchar", ct::c_uchar},
        {"c_float", ct::c_float},
        {"boolean", ct::c_bool},
        {"userdata", ct::c_void_ptr},
        {"number", ct::c_double},
        {"void", ct::c_void},
        {"string", ct::c_string},
    };
    if (not map.contains(str)) return std::nullopt;
    return map.at(str);
}
int cinterop::new_function_binding(lua_State* L) {
    Dlmodule* module = dlimport::lua_tomodule(L, 1);
    C_func_binding binding{};
    const int top = lua_gettop(L);
    if (cinterop::is_struct_info(L, 2)) {
        auto& si = cinterop::to_struct_info(L, 2);
        binding.types.push_back(si);
    } else {
        Strview return_type = luaL_checkstring(L, 2);
        auto res = string_to_param_type(return_type);
        if (not res) luaL_argerrorL(L, 2, "not a c type");
        binding.types.push_back(std::move(*res));
    }
    auto proc = dlimport::find_proc_address(*module, luaL_checkstring(L, 3));
    if (not proc) luaL_argerrorL(L, 3, "couldn't find address");
    binding.function_pointer = *proc;
    if (top > 3) {
        for (int i{4}; i <= top; ++i) {
            if (cinterop::is_struct_info(L, i)) {
                auto& si = cinterop::to_struct_info(L, i);
                binding.types.push_back(si);
                continue;
            }
            auto res = string_to_param_type(luaL_checkstring(L, i));
            if (not res) luaL_argerrorL(L, i, "not a c type");
            else if (*res == C_type::c_void) luaL_argerrorL(L, i, "an argument type cannot be void.");
            binding.types.push_back(std::move(*res));
        }
    }
    C_func_binding* up = static_cast<C_func_binding*>(lua_newuserdatadtor(L, sizeof(C_func_binding), [](void* ud) {
        static_cast<C_func_binding*>(ud)->~C_func_binding();
    }));
    new (up) C_func_binding{std::move(binding)};
    lua_pushcclosure(L, call_binding, "c_function_binding", 1);
    return 1;
}
