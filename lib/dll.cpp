#include "goluau.h"
#include <lualib.h>
#include <unordered_map>
#include <string>
#include <lualib.h>
#include <luacode.h>
#include <Luau/Common.h>
#include <memory>
#include <cstdint>
#include <optional>
#include <ranges>
#include <algorithm>
#include <Windows.h>
#include <cassert>
#include <boost/container/flat_map.hpp>
#include <core.hpp>
#include <format>
#include <filesystem>
#include <dyncall.h>
#include <iostream>
#include <format>
namespace fs = std::filesystem;
namespace rn = std::ranges;
using String = std::string;
template <class T, class D = std::default_delete<T>>
using Unique_ptr = std::unique_ptr<T, D>;
using std::make_unique;
template<class K, class V>
using Flat_map = boost::container::flat_map<K, V>;
using Pointer = std::uintptr_t;
template <class T>
using Vector = std::vector<T>;
using String_view = std::string_view;
template<class T>
using Optional = std::optional<T>;
using std::make_optional;
using std::nullopt;

static int lutag{};
static std::unique_ptr<DCCallVM, decltype(&dcFree)> call_vm{dcNewCallVM(1024), dcFree};
static int getfunc_stringatom{};
static int dyncall_void_stringatom{};
static int dyncall_int_stringatom{};
static int create_binding_stringatom{};
constexpr const char* tname{"dllmodule"};
struct Module {
    using Handle = HMODULE;
    Unique_ptr<DCCallVM, decltype(&dcFree)> vm;
    Handle handle;
    String name;
    String path;
    Module(Handle handle, String name, String path):
        handle(handle),
        name(name),
        path(path),
        vm(nullptr, dcFree) {}
    ~Module() {if (handle) FreeLibrary(handle);}
    Flat_map<String, Pointer> cached{};
};
static Flat_map<String, Unique_ptr<Module>> loaded_modules{};
Optional<String> find_module_path(const String& dllname) {
    char buffer[MAX_PATH];
    DWORD result = SearchPath(
        nullptr,       // Search in standard locations
        dllname.c_str(), // Name of the DLL
        nullptr,       // File extension (optional)
        MAX_PATH,      // Buffer size
        buffer,        // Buffer to store the result
        nullptr        // Ignored (no filename extension needed)
    );
    if (result == 0 or result > MAX_PATH) {
        if (dllname.ends_with(".dll")) {
            for (const auto& [state, path] : script_path_registry) {
                fs::path dllpath = fs::path(path).parent_path() / dllname;
                if (fs::exists(dllpath)) return fs::absolute(dllpath).string();
            }
            return std::nullopt;
        }
        return find_module_path(dllname + ".dll");
    }
    String path{buffer};
    rn::replace(path, '\\', '/');
    return path;
}
static Module* init_or_find_module(const String& name) {
    auto found_path = find_module_path(name);
    if (not found_path) return nullptr;
    if (auto it = loaded_modules.find(*found_path); it == loaded_modules.end()) {
        HMODULE hm = LoadLibrary(found_path->c_str());
        if (not hm) [[unlikely]] return nullptr;
        loaded_modules.emplace(*found_path, std::make_unique<Module>(hm, name, *found_path));
    }
    return loaded_modules[*found_path].get();
}
static Optional<Pointer> find_proc_address(Module& module, const String& symbol) {
    auto& cached = module.cached;
    if (auto found = cached.find(symbol); found != cached.end()) return cached.at(symbol);
    FARPROC proc = GetProcAddress(module.handle, symbol.c_str());
    if (proc) cached.emplace(symbol, reinterpret_cast<Pointer>(proc));
    return proc ? make_optional(reinterpret_cast<Pointer>(proc)) : nullopt;
}
static Module* lua_tomodule(lua_State* L, int idx) {
    if (lua_lightuserdatatag(L, idx) != lutag) luaL_typeerrorL(L, idx, tname);
    auto mod = static_cast<Module*>(lua_tolightuserdatatagged(L, idx, lutag));
    return mod;
}
static Module* lua_pushmodule(lua_State* L, Module* module) {
    lua_pushlightuserdatatagged(L, module, lutag);
    luaL_getmetatable(L, tname);
    lua_setmetatable(L, -2);
    return module;
}
static int findpath(lua_State* L) {
    if (auto path = find_module_path(luaL_checkstring(L, 1))) {
        lua_pushstring(L, path->c_str());
        return 1;
    }
    return 0;
}
static int load(lua_State* L) {
    Module* module = init_or_find_module(luaL_checkstring(L, 1));
    if (not module) luaL_argerrorL(L, 1, "couldn't find dll");
    lua_pushmodule(L, module);
    return 1;
}
static int dyncall_void(lua_State* L) {
    auto* module = lua_tomodule(L, 1);
    auto proc = find_proc_address(*module, luaL_checkstring(L, 2));
    if (not proc) luaL_argerrorL(L, 2, "couldnt find address");
    if (not module->vm) {
        constexpr DCsize default_size{1024};
        module->vm.reset(dcNewCallVM(default_size));
    }
    DCCallVM* vm = module->vm.get();
    dcReset(vm);
    dcCallVoid(vm, reinterpret_cast<DCpointer>(*proc));
    return 0;
}
enum class ParamType {
    Int, Void, Float, String, Double
};
struct Binding {
    Vector<ParamType> types;
    ParamType get_return_type() const {return types[0];}
    Pointer function_pointer;
};
static int call_binding(lua_State* L) {
    const Binding& binding = *static_cast<Binding*>(lua_touserdata(L, lua_upvalueindex(1)));
    std::cout << std::format("CALLING s{} fn{}\n", binding.types.size(), binding.function_pointer);
    DCCallVM* vm = call_vm.get();
    dcReset(vm);
    using Pt = ParamType;
    for (int i{1}; i < binding.types.size(); ++i) {
        std::cout << std::format("Arg {} {}\n", i, int(binding.types.at(i)));
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
    std::cout << std::format("Return {}\n", int(binding.types.at(0)));
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
static Optional<ParamType> string_to_param_type(String_view str) {
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
    return nullopt;
}
static int create_binding(lua_State* L) {
    Module* module = lua_tomodule(L, 1);
    Binding binding{};
    const int top = lua_gettop(L);
    String_view return_type = luaL_checkstring(L, 2);
    using Pt = ParamType;
    auto res = string_to_param_type(return_type);
    if (not res) luaL_argerrorL(L, 2, "not a c type");
    binding.types.push_back(std::move(*res));

    auto proc = find_proc_address(*module, luaL_checkstring(L, 3));
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
        std::cout << "DESTROYING\n";
        static_cast<Binding*>(ud)->~Binding();
    }));
    new (up) Binding{std::move(binding)};
    std::cout << std::format("s{} fn{}\n", up->types.size(), up->function_pointer);
    lua_pushcclosure(L, call_binding, "call_binding", 1);
    return 1;
}
static int dyncall_int(lua_State* L) {
    auto* module = lua_tomodule(L, 1);
    auto proc = find_proc_address(*module, luaL_checkstring(L, 2));
    if (not proc) luaL_argerrorL(L, 2, "couldnt find address");
    if (not module->vm) {
        constexpr DCsize default_size{1024};
        module->vm.reset(dcNewCallVM(default_size));
    }
    DCCallVM* vm = module->vm.get();
    const int argn = lua_gettop(L);
    dcReset(vm);
    if (argn > 2) {
        for (int i{3}; i <= argn; ++i) {
            if (lua_isnumber(L, i)) dcArgDouble(vm, lua_tonumber(L, i));
            else if (lua_isstring(L, i)) dcArgPointer(vm, (void*)lua_tostring(L, i));
            else luaL_argerrorL(L, i, "invalid type");
        }
    }
    lua_pushinteger(L, dcCallInt(vm, reinterpret_cast<DCpointer>(*proc)));
    return 1;
}
static int cfunction(lua_State* L) {
    const char* proc_key = luaL_checkstring(L, 2);
    auto opt = find_proc_address(*lua_tomodule(L, 1), proc_key);
    if (not opt) luaL_errorL(L, "function was not found ");
    const auto fmt = std::format("{}", proc_key);
    lua_pushcfunction(L, reinterpret_cast<lua_CFunction>(*opt), fmt.c_str());
    return 1;
}
static int find(lua_State* L) {
    auto module = init_or_find_module(luaL_checkstring(L, 1));
    if (not module) return 0;
    lua_pushmodule(L, module);
    return 1;
}
// kind of unsafe
static int unload(lua_State* L) {
    if (auto path = find_module_path(luaL_checkstring(L, 1))) {
        auto found_it = loaded_modules.find(*path);
        if (found_it == loaded_modules.end()) return 0;
        loaded_modules.erase(*path);
    }
    return 0;
}
static int loaded(lua_State* L) {
    lua_newtable(L);
    int i{1};
    for (const auto& [key, v] : loaded_modules) {
        lua_pushstring(L, key.c_str());
        lua_rawseti(L, -2, i++);
    }
    return 1;
}
static int module_index(lua_State* L) {
    Module* module = lua_tomodule(L, 1);
    const std::string_view key = luaL_checkstring(L, 2);
    if (key == "path") {
        lua_pushstring(L, module->path.c_str());
        return 1;
    } else if (key == "name") {
        lua_pushstring(L, module->name.c_str());
        return 1;
    }
    luaL_argerrorL(L, 2, "index was null");
}
static int module_namecall(lua_State* L) {
    Module& module = *lua_tomodule(L, 1);
    int atom;
    lua_namecallatom(L, &atom);
    if (atom == getfunc_stringatom) return cfunction(L);
    else if(atom == dyncall_void_stringatom) return dyncall_void(L);
    else if(atom == dyncall_int_stringatom) return dyncall_int(L);
    else if(atom == create_binding_stringatom) return create_binding(L);
    luaL_errorL(L, "invalid");
}
int goluauload_dll(lua_State* L) {
    if (luaL_newmetatable(L, tname)) {
        lutag = goluau_newludtag();
        getfunc_stringatom = goluau_stringatom(L, "cfunction");
        dyncall_void_stringatom = goluau_stringatom(L, "dyncall_void");
        dyncall_int_stringatom = goluau_stringatom(L, "dyncall_int");
        create_binding_stringatom = goluau_stringatom(L, "create_binding");
        lua_setlightuserdataname(L, lutag, tname);
        const luaL_Reg meta[] = {
            {"__index", module_index},
            {"__namecall", module_namecall},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
    }
    lua_pop(L, 1);
    const luaL_Reg lib[] = {
        {"load", load},
        {"find", find},
        {"cfunction", cfunction},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    return 1;
}
void goluauopen_dll(lua_State* L) {
    goluauload_dll(L);
    lua_setglobal(L, "dll");
}
