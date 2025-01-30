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
#include <dluau.h>

struct dlmodule {
    using alias = HMODULE;
    alias handle;
    std::string name;
    std::string path;
    dlmodule(alias handle, std::string name, std::string path):
        handle(handle),
        name(name),
        path(path) {}
    ~dlmodule() {if (handle) FreeLibrary(handle);}
    boost::container::flat_map<std::string, uintptr_t> cached{};
    inline static const int tag{dluau_newlightuserdatatag()};
    constexpr static const char* tname{"dlmodule"};
    static int create_binding(lua_State* L);
    static void init(lua_State* L);
};
namespace glob {
inline boost::container::flat_map<std::string, std::unique_ptr<dlmodule>> loaded;
inline std::unique_ptr<DCCallVM, decltype(&dcFree)> call_vm{dcNewCallVM(1024), dcFree};
}
namespace util {
std::optional<std::string> find_module_path(const std::string& dlpath);
dlmodule* init_or_find_module(const std::string& name);
std::optional<uintptr_t> find_proc_address(dlmodule& module, const std::string& symbol);
dlmodule* lua_tomodule(lua_State* L, int idx);
dlmodule* lua_pushmodule(lua_State* L, dlmodule* module);
}
