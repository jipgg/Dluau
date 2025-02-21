#include "cinterop.hpp"
#include <variant>
#include <vector>
#include <bit>
using std::unique_ptr, std::shared_ptr;
using std::vector, std::variant;
using std::string, std::string_view;
namespace bc = boost::container;
using namespace cinterop;
using Uint = unsigned int;
using Ushort = unsigned short;
using Ulong = unsigned long;
using Uchar = unsigned char;

static std::unique_ptr<DCCallVM, decltype(&dcFree)> call_vm{dcNewCallVM(1024), dcFree};

struct C_function_binding {
    vector<variant<Native_type, shared_ptr<Struct_info>>> types;
    variant<Native_type, shared_ptr<Struct_info>> get_return_type() const {return types[0];}
    uintptr_t function_pointer;
};
template <class Ty>
constexpr auto check_num(lua_State* L, int idx) -> Ty {
    return static_cast<Ty>(luaL_checknumber(L, idx));
}
static auto call_binding(lua_State* L) -> int {
    const auto& binding = *static_cast<C_function_binding*>(lua_touserdata(L, lua_upvalueindex(1)));
    DCCallVM* vm = call_vm.get();
    dcReset(vm);
    int curr_aggr{};
    for (int i{1}; i < binding.types.size(); ++i) {
        if (auto* type = std::get_if<Native_type>(&binding.types[i])) {
            switch(*type) {
                case Native_type::Bool:
                    dcArgBool(vm, luaL_checkboolean(L, i));
                    break;
                case Native_type::Int:
                    dcArgInt(vm, luaL_checkinteger(L, i));
                    break;
                case Native_type::Uint:
                    dcArgInt(vm, check_num<Uint>(L, i));
                    break;
                case Native_type::Short:
                    dcArgShort(vm, check_num<short>(L, i));
                    break;
                case Native_type::Ushort:
                    dcArgShort(vm, check_num<Ushort>(L, i));
                    break;
                case Native_type::Char:
                    dcArgChar(vm, check_num<char>(L, i));
                    break;
                case Native_type::Uchar:
                    dcArgChar(vm, check_num<Uchar>(L, i));
                    break;
                case Native_type::Long:
                    dcArgLong(vm, check_num<long>(L, i));
                    break;
                case Native_type::Ulong:
                    dcArgLong(vm, check_num<Ulong>(L, i));
                    break;
                case Native_type::Float:
                    dcArgFloat(vm, check_num<float>(L, i));
                    break;
                case Native_type::Double:
                    dcArgDouble(vm, luaL_checknumber(L, i));
                    break;
                case Native_type::Char_ptr:
                    dcArgPointer(vm, (DCpointer)luaL_checkstring(L, i));
                    break;
                case Native_type::Void_ptr:
                    if (lua_islightuserdata(L, i)) {
                        dcArgPointer(vm, lua_tolightuserdata(L, i));
                    } else if (lua_isuserdata(L, i)) {
                        dcArgPointer(vm, lua_touserdata(L, i));
                    } else dluau::type_error(L, i, "not userdata or opaque");
                    break;
                case Native_type::Void:
                    break;
            }
        } else {
            auto& si = std::get<std::shared_ptr<Struct_info>>(binding.types[i]);
            void* data = lua_touserdata(L, i);
            dcArgAggr(vm, si->aggr.get(), data);
        }
    }
    DCpointer fnptr = reinterpret_cast<DCpointer>(binding.function_pointer);
    if (auto* type = std::get_if<Native_type>(&binding.types.at(0))) {
        switch(*type) {
            case Native_type::Bool:
                lua_pushboolean(L, dcCallBool(vm, fnptr));
                return 1;
            case Native_type::Int:
                lua_pushnumber(L, dcCallInt(vm, fnptr));
                return 1;
            case Native_type::Uint:
                lua_pushnumber(L, std::bit_cast<Uint>(dcCallInt(vm, fnptr)));
                return 1;
            case Native_type::Short:
                lua_pushnumber(L, dcCallShort(vm, fnptr));
                return 1;
            case Native_type::Ushort:
                lua_pushnumber(L, std::bit_cast<Ushort>(dcCallShort(vm, fnptr)));
                return 1;
            case Native_type::Long:
                lua_pushnumber(L, dcCallLong(vm, fnptr));
                return 1;
            case Native_type::Ulong:
                lua_pushnumber(L, std::bit_cast<Ulong>(dcCallLong(vm, fnptr)));
                return 1;
            case Native_type::Char:
                lua_pushnumber(L, dcCallChar(vm, fnptr));
                return 1;
            case Native_type::Uchar:
                lua_pushnumber(L, std::bit_cast<Uchar>(dcCallChar(vm, fnptr)));
                return 1;
            case Native_type::Double:
                lua_pushnumber(L, dcCallDouble(vm, fnptr));
                return 1;
            case Native_type::Float:
                lua_pushnumber(L, dcCallFloat(vm, fnptr));
                return 1;
            case Native_type::Char_ptr:
                lua_pushstring(L, static_cast<char*>(dcCallPointer(vm, fnptr)));
                return 1;
            case Native_type::Void_ptr:
                lua_pushlightuserdata(L, dcCallPointer(vm, fnptr));
                return 1;
            case Native_type::Void:
                dcCallVoid(vm, fnptr);
                return 0;
        }
    } else {
        auto& si = std::get<shared_ptr<Struct_info>>(binding.types[0]);
        //void* buf = lua_newbuffer(L, si->memory_size);
        void* instance = si->newinstance(L);
        dcCallAggr(vm, fnptr, si->aggr.get(), instance);
        return 1;
    }
    luaL_errorL(L, "call error");
}
auto string_to_param_type(std::string_view str) -> std::optional<Native_type> {
    static const bc::flat_map<std::string_view, Native_type> map {
        {"int", Native_type::Int},
        {"uint", Native_type::Uint},
        {"short", Native_type::Short},
        {"ushort", Native_type::Ushort},
        {"long", Native_type::Long},
        {"ulong", Native_type::Ulong},
        {"char", Native_type::Char},
        {"uchar", Native_type::Uchar},
        {"float", Native_type::Float},
        {"bool", Native_type::Bool},
        {"void*", Native_type::Void_ptr},
        {"double", Native_type::Double},
        {"void", Native_type::Void},
        {"char*", Native_type::Char_ptr},
    };
    if (not map.contains(str)) return std::nullopt;
    return map.at(str);
}
auto cinterop::new_function_binding(lua_State* L) -> int {
    dluau_Dlmodule* module = dluau_todlmodule(L, 1);
    C_function_binding binding{};
    const int top = lua_gettop(L);
    if (Struct_info_type::is(L, 2)) {
        auto& si = Struct_info_type::view(L, 2);
        binding.types.push_back(si);
    } else {
        string_view return_type = luaL_checkstring(L, 2);
        auto res = string_to_param_type(return_type);
        if (not res) luaL_argerrorL(L, 2, "not a c type");
        binding.types.push_back(std::move(*res));
    }
    auto proc = dluau_dlfindprocaddress(module, luaL_checkstring(L, 3));
    if (proc == 0) luaL_argerrorL(L, 3, "couldn't find address");
    binding.function_pointer = proc;
    if (top > 3) {
        for (int i{4}; i <= top; ++i) {
            if (Struct_info_type::is(L, i)) {
                auto& si = Struct_info_type::view(L, i);
                binding.types.push_back(si);
                continue;
            }
            auto res = string_to_param_type(luaL_checkstring(L, i));
            if (not res) luaL_argerrorL(L, i, "not a c type");
            else if (*res == Native_type::Void) luaL_argerrorL(L, i, "an argument type cannot be void.");
            binding.types.push_back(std::move(*res));
        }
    }
    C_function_binding* up = static_cast<C_function_binding*>(lua_newuserdatadtor(L, sizeof(C_function_binding), [](void* ud) {
        static_cast<C_function_binding*>(ud)->~C_function_binding();
    }));
    new (up) C_function_binding{std::move(binding)};
    lua_pushcclosure(L, call_binding, "c_function_binding", 1);
    return 1;
}
