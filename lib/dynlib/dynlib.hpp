#pragma once
#ifdef _WIN32
#include <Windows.h>
#endif
#include <boost/container/flat_map.hpp>
#include <dyncall.h>
#include <memory>
#include <string>
#include <string_view>
#include <optional>
#include <lualib.h>
#include <goluau.h>
using String = std::string;
using StrView = std::string_view;
template <class Ty>
using Opt = std::optional<Ty>;
namespace bc = boost::container;

struct Dllmodule {
    using Handle = HMODULE;
    Handle handle;
    String name;
    String path;
    Dllmodule(Handle handle, String name, String path):
        handle(handle),
        name(name),
        path(path) {}
    ~Dllmodule() {if (handle) FreeLibrary(handle);}
    bc::flat_map<String, uintptr_t> cached{};
    inline static const int tag{goluau_newludtag()};
    constexpr static const char* tname{"dynmodule"};
    static int create_binding(lua_State* L);
    static void init(lua_State* L);
};
namespace glob {
inline bc::flat_map<String, std::unique_ptr<Dllmodule>> loaded;
inline std::unique_ptr<DCCallVM, decltype(&dcFree)> call_vm{dcNewCallVM(1024), dcFree};
}
namespace util {
Opt<String> find_module_path(const String& dllname);
Dllmodule* init_or_find_module(const String& name);
Opt<uintptr_t> find_proc_address(Dllmodule& module, const String& symbol);
Dllmodule* lua_tomodule(lua_State* L, int idx);
Dllmodule* lua_pushmodule(lua_State* L, Dllmodule* module);
}
