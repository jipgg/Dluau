#include "cinterop.hpp"
#include <variant>
#include <vector>
#include <bit>
using std::unique_ptr, std::shared_ptr;
using std::vector, std::variant;
using std::string, std::string_view;
namespace bc = boost::container;
using namespace cinterop;
using uint = unsigned int;
using ushort = unsigned short;
using ulong = unsigned long;
using uchar = unsigned char;

static std::unique_ptr<DCCallVM, decltype(&dcFree)> call_vm{dcNewCallVM(1024), dcFree};

struct CFunctionBinding {
    vector<variant<NativeType, shared_ptr<StructInfo>>> types;
    variant<NativeType, shared_ptr<StructInfo>> get_return_type() const {return types[0];}
    uintptr_t function_pointer;
};
template <class Ty>
constexpr auto check_num(lua_State* L, int idx) -> Ty {
    return static_cast<Ty>(luaL_checknumber(L, idx));
}
static auto call_binding(lua_State* L) -> int {
    const auto& binding = *static_cast<CFunctionBinding*>(lua_touserdata(L, lua_upvalueindex(1)));
    DCCallVM* vm = call_vm.get();
    dcReset(vm);
    int curr_aggr{};
    for (int i{1}; i < binding.types.size(); ++i) {
        if (auto* type = std::get_if<NativeType>(&binding.types[i])) {
            switch(*type) {
                case NativeType::Bool:
                    dcArgBool(vm, luaL_checkboolean(L, i));
                    break;
                case NativeType::Int:
                    dcArgInt(vm, luaL_checkinteger(L, i));
                    break;
                case NativeType::Uint:
                    dcArgInt(vm, check_num<uint>(L, i));
                    break;
                case NativeType::Short:
                    dcArgShort(vm, check_num<short>(L, i));
                    break;
                case NativeType::Ushort:
                    dcArgShort(vm, check_num<ushort>(L, i));
                    break;
                case NativeType::Char:
                    dcArgChar(vm, check_num<char>(L, i));
                    break;
                case NativeType::Uchar:
                    dcArgChar(vm, check_num<uchar>(L, i));
                    break;
                case NativeType::Long:
                    dcArgLong(vm, check_num<long>(L, i));
                    break;
                case NativeType::Ulong:
                    dcArgLong(vm, check_num<ulong>(L, i));
                    break;
                case NativeType::Float:
                    dcArgFloat(vm, check_num<float>(L, i));
                    break;
                case NativeType::Double:
                    dcArgDouble(vm, luaL_checknumber(L, i));
                    break;
                case NativeType::String:
                    dcArgPointer(vm, (DCpointer)luaL_checkstring(L, i));
                    break;
                case NativeType::VoidPtr:
                    if (not lua_islightuserdata(L, i)) luaL_typeerrorL(L, i, "not light userdata.");
                    dcArgPointer(vm, lua_tolightuserdata(L, i));
                    break;
                case NativeType::Void:
                    break;
            }
        } else {
            auto& si = std::get<std::shared_ptr<StructInfo>>(binding.types[i]);
            void* data = lua_touserdata(L, i);
            dcArgAggr(vm, si->aggr.get(), data);
        }
    }
    DCpointer fnptr = reinterpret_cast<DCpointer>(binding.function_pointer);
    if (auto* type = std::get_if<NativeType>(&binding.types.at(0))) {
        switch(*type) {
            case NativeType::Bool:
                lua_pushboolean(L, dcCallBool(vm, fnptr));
                return 1;
            case NativeType::Int:
                lua_pushnumber(L, dcCallInt(vm, fnptr));
                return 1;
            case NativeType::Uint:
                lua_pushnumber(L, std::bit_cast<uint>(dcCallInt(vm, fnptr)));
                return 1;
            case NativeType::Short:
                lua_pushnumber(L, dcCallShort(vm, fnptr));
                return 1;
            case NativeType::Ushort:
                lua_pushnumber(L, std::bit_cast<ushort>(dcCallShort(vm, fnptr)));
                return 1;
            case NativeType::Long:
                lua_pushnumber(L, dcCallLong(vm, fnptr));
                return 1;
            case NativeType::Ulong:
                lua_pushnumber(L, std::bit_cast<ulong>(dcCallLong(vm, fnptr)));
                return 1;
            case NativeType::Char:
                lua_pushnumber(L, dcCallChar(vm, fnptr));
                return 1;
            case NativeType::Uchar:
                lua_pushnumber(L, std::bit_cast<uchar>(dcCallChar(vm, fnptr)));
                return 1;
            case NativeType::Double:
                lua_pushnumber(L, dcCallDouble(vm, fnptr));
                return 1;
            case NativeType::Float:
                lua_pushnumber(L, dcCallFloat(vm, fnptr));
                return 1;
            case NativeType::String:
                lua_pushstring(L, static_cast<char*>(dcCallPointer(vm, fnptr)));
                return 1;
            case NativeType::VoidPtr:
                lua_pushlightuserdata(L, dcCallPointer(vm, fnptr));
                return 1;
            case NativeType::Void:
                dcCallVoid(vm, fnptr);
                return 0;
        }
    } else {
        auto& si = std::get<shared_ptr<StructInfo>>(binding.types[0]);
        //void* buf = lua_newbuffer(L, si->memory_size);
        void* instance = si->newinstance(L);
        dcCallAggr(vm, fnptr, si->aggr.get(), instance);
        return 1;
    }
    luaL_errorL(L, "call error");
}
auto string_to_param_type(std::string_view str) -> std::optional<NativeType> {
    static const bc::flat_map<std::string_view, NativeType> map {
        {"int", NativeType::Int},
        {"uint", NativeType::Uint},
        {"short", NativeType::Short},
        {"ushort", NativeType::Ushort},
        {"long", NativeType::Long},
        {"ulong", NativeType::Ulong},
        {"char", NativeType::Char},
        {"uchar", NativeType::Uchar},
        {"float", NativeType::Float},
        {"boolean", NativeType::Bool},
        {"userdata", NativeType::VoidPtr},
        {"number", NativeType::Double},
        {"void", NativeType::Void},
        {"string", NativeType::String},
    };
    if (not map.contains(str)) return std::nullopt;
    return map.at(str);
}
auto cinterop::new_function_binding(lua_State* L) -> int {
    dluau_Dlmodule* module = dluau_todlmodule(L, 1);
    CFunctionBinding binding{};
    const int top = lua_gettop(L);
    if (StructInfoType::is(L, 2)) {
        auto& si = StructInfoType::view(L, 2);
        binding.types.push_back(si);
    } else {
        string_view return_type = luaL_checkstring(L, 2);
        auto res = string_to_param_type(return_type);
        if (not res) luaL_argerrorL(L, 2, "not a c type");
        binding.types.push_back(std::move(*res));
    }
    auto proc = dluau_dlmodulefind(module, luaL_checkstring(L, 3));
    if (proc == 0) luaL_argerrorL(L, 3, "couldn't find address");
    binding.function_pointer = proc;
    if (top > 3) {
        for (int i{4}; i <= top; ++i) {
            if (StructInfoType::is(L, i)) {
                auto& si = StructInfoType::view(L, i);
                binding.types.push_back(si);
                continue;
            }
            auto res = string_to_param_type(luaL_checkstring(L, i));
            if (not res) luaL_argerrorL(L, i, "not a c type");
            else if (*res == NativeType::Void) luaL_argerrorL(L, i, "an argument type cannot be void.");
            binding.types.push_back(std::move(*res));
        }
    }
    CFunctionBinding* up = static_cast<CFunctionBinding*>(lua_newuserdatadtor(L, sizeof(CFunctionBinding), [](void* ud) {
        static_cast<CFunctionBinding*>(ud)->~CFunctionBinding();
    }));
    new (up) CFunctionBinding{std::move(binding)};
    lua_pushcclosure(L, call_binding, "c_function_binding", 1);
    return 1;
}
