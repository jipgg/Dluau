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

struct Dlmodule {
    using Handle = HMODULE;
    Handle handle;
    String name;
    String path;
    Dlmodule(Handle handle, String name, String path):
        handle(handle),
        name(name),
        path(path) {}
    ~Dlmodule() {if (handle) FreeLibrary(handle);}
    bc::flat_map<String, uintptr_t> cached{};
    inline static const int tag{goluau_newludtag()};
    constexpr static const char* tname{"dlmodule"};
    static int create_binding(lua_State* L);
    static void init(lua_State* L);
};
namespace glob {
inline bc::flat_map<String, std::unique_ptr<Dlmodule>> loaded;
inline std::unique_ptr<DCCallVM, decltype(&dcFree)> call_vm{dcNewCallVM(1024), dcFree};
}
namespace util {
Opt<String> find_module_path(const String& dllname);
Dlmodule* init_or_find_module(const String& name);
Opt<uintptr_t> find_proc_address(Dlmodule& module, const String& symbol);
Dlmodule* lua_tomodule(lua_State* L, int idx);
Dlmodule* lua_pushmodule(lua_State* L, Dlmodule* module);
}
