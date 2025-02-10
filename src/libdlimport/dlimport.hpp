#pragma once
#ifdef _WIN32
#include <Windows.h>
#endif
#include <boost/container/flat_map.hpp>
#include <dyncall.h>
#include <filesystem>
#include <memory>
#include <string>
#include <variant>
#include <optional>
#include <dluau.hpp>
using namespace dluau::type_aliases;

namespace dlimport {
struct Dlmodule {
    using Handle = HMODULE;
    Handle handle;
    String name;
    Path path;
    Dlmodule(Handle handle, String name, Path path):
        handle(handle),
        name(std::move(name)),
        path(std::move(path)) {}
    ~Dlmodule() {if (handle) FreeLibrary(handle);}
    Flat_map<String, uintptr_t> cached{};
    inline static const int tag{dluau_newlightuserdatatag()};
    constexpr static const char* tname{"dlmodule"};
    static void init(Lstate L);
};

using Dlmodule_map = Flat_map<Path, Unique<Dlmodule>>;
const Dlmodule_map& get_dlmodules();
Dlmodule* find_module(const String& name);
Expected<Ref<Dlmodule>> init_module(const Path& path);
Expected<Ref<Dlmodule>> load_module(Lstate L);
Opt<uintptr_t> find_proc_address(Dlmodule& module, const String& symbol);
Dlmodule* lua_tomodule(Lstate L, int idx);
Dlmodule* lua_pushmodule(Lstate L, Dlmodule* module);
Opt<Path> search_path(const Path& dlpath);
}
