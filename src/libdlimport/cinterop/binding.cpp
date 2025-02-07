#include "cinterop.hpp"
using std::optional, std::string_view;

static std::unique_ptr<DCCallVM, decltype(&dcFree)> call_vm{dcNewCallVM(1024), dcFree};

struct c_function_binding {
    aggregate_sp return_aggr;
    std::vector<aggregate_sp> aggregates;
    std::vector<c_type> types;
    c_type get_return_type() const {return types[0];}
    uintptr_t function_pointer;
};
static int call_binding(lua_State* L) {
    const auto& binding = *static_cast<c_function_binding*>(lua_touserdata(L, lua_upvalueindex(1)));
    DCCallVM* vm = call_vm.get();
    dcReset(vm);
    using ct = c_type;
    int curr_aggr{};
    auto next_aggr = [&curr_aggr, &ags = binding.aggregates] {
        return ags.at(curr_aggr++)->get();
    };
    for (int i{1}; i < binding.types.size(); ++i) {
        switch(binding.types.at(i)) {
            case ct::c_bool:
                dcArgBool(vm, luaL_checkboolean(L, i));
                break;
            case ct::c_int:
                dcArgInt(vm, dluau_checkc_int(L, i));
                break;
            case ct::c_uint:
                dcArgInt(vm, dluau_checkc_uint(L, i));
                break;
            case ct::c_short:
                dcArgShort(vm, dluau_checkc_short(L, i));
                break;
            case ct::c_ushort:
                dcArgShort(vm, dluau_checkc_ushort(L, i));
                break;
            case ct::c_char:
                dcArgChar(vm, dluau_checkc_char(L, i));
                break;
            case ct::c_uchar:
                dcArgChar(vm, dluau_checkc_uchar(L, i));
                break;
            case ct::c_long:
                dcArgLong(vm, dluau_checkc_long(L, i));
                break;
            case ct::c_ulong:
                dcArgLong(vm, dluau_checkc_ulong(L, i));
                break;
            case ct::c_float:
                dcArgFloat(vm, dluau_checkc_float(L, i));
                break;
            case ct::c_double:
                dcArgDouble(vm, luaL_checknumber(L, i));
                break;
            case ct::c_char_ptr:
                dcArgPointer(vm, (DCpointer)luaL_checkstring(L, i));
                break;
            case ct::c_void_ptr:
                if (not lua_islightuserdata(L, i)) luaL_typeerrorL(L, i, "not light userdata.");
                dcArgPointer(vm, lua_tolightuserdata(L, i));
                break;
            case ct::c_aggregate: {
                size_t len;
                //should error handle bufgfer size
                dcArgAggr(vm, next_aggr(), luaL_checkbuffer(L, i, &len));
                break;
            } case ct::c_void:
                break;
        }
    }
    DCpointer fnptr = reinterpret_cast<DCpointer>(binding.function_pointer);
    switch(binding.types.at(0)) {
        case ct::c_bool:
            lua_pushboolean(L, dcCallBool(vm, fnptr));
            return 1;
        case ct::c_int:
            dluau_pushc_int(L, dcCallInt(vm, fnptr));
            return 1;
        case ct::c_uint:
            dluau_pushc_uint(L, static_cast<unsigned int>(dcCallInt(vm, fnptr)));
            return 1;
        case ct::c_short:
            dluau_pushc_short(L, dcCallShort(vm, fnptr));
            return 1;
        case ct::c_ushort:
            dluau_pushc_ushort(L, (unsigned short)dcCallShort(vm, fnptr));
            return 1;
        case ct::c_long:
            dluau_pushc_long(L, dcCallLong(vm, fnptr));
            return 1;
        case ct::c_ulong:
            dluau_pushc_ulong(L, (unsigned long)dcCallLong(vm, fnptr));
            return 1;
        case ct::c_char:
            dluau_pushc_char(L, dcCallChar(vm, fnptr));
            return 1;
        case ct::c_uchar:
            dluau_pushc_uchar(L, (unsigned char)dcCallChar(vm, fnptr));
            return 1;
        case ct::c_double:
            lua_pushnumber(L, dcCallDouble(vm, fnptr));
            return 1;
        case ct::c_float:
            dluau_pushc_float(L, dcCallFloat(vm, fnptr));
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
        case ct::c_aggregate: {
            auto& aggr = *binding.return_aggr;
            void* buf = lua_newbuffer(L, aggr.size());
            dcCallAggr(vm, fnptr, aggr.get(), buf);
        }
    }
    luaL_errorL(L, "call error");
}
optional<c_type> string_to_param_type(string_view str) {
    using ct = c_type;
    static const boost::container::flat_map<string_view, c_type> map {
        {"c_int", ct::c_int},
        {"c_uint", ct::c_uint},
        {"c_short", ct::c_short},
        {"c_ushort", ct::c_ushort},
        {"c_long", ct::c_short},
        {"c_ulong", ct::c_ulong},
        {"c_char", ct::c_char},
        {"c_uchar", ct::c_uchar},
        {"boolean", ct::c_bool},
        {"userdata", ct::c_void_ptr},
        {"number", ct::c_double},
        {"void", ct::c_void},
        {"string", ct::c_char_ptr},
        {"c_aggregate", ct::c_aggregate},
    };
    if (not map.contains(str)) return std::nullopt;
    return map.at(str);
}
int cinterop::new_function_binding(lua_State* L) {
    dlmodule* module = dlimport::lua_tomodule(L, 1);
    c_function_binding binding{};
    const int top = lua_gettop(L);
    if (lua_userdatatag(L, 2) == aggregate_tag) {
        binding.return_aggr = *static_cast<aggregate_sp*>(lua_touserdatatagged(L, 2, aggregate_tag));
        binding.types.push_back(c_type::c_aggregate);
    } else {
        string_view return_type = luaL_checkstring(L, 2);
        auto res = string_to_param_type(return_type);
        if (not res) luaL_argerrorL(L, 2, "not a c type");
        binding.types.push_back(std::move(*res));
    }
    auto proc = dlimport::find_proc_address(*module, luaL_checkstring(L, 3));
    if (not proc) luaL_argerrorL(L, 3, "couldn't find address");
    binding.function_pointer = *proc;
    if (top > 3) {
        for (int i{4}; i <= top; ++i) {
            if (lua_userdatatag(L, i) == aggregate_tag) {
                binding.aggregates.push_back(*static_cast<aggregate_sp*>(lua_touserdatatagged(L, i, aggregate_tag)));
                binding.types.push_back(c_type::c_aggregate);
                continue;
            }
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
