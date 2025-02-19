#pragma once
#include <std.hpp>
#include <filesystem>
#include <memory>
#include <iostream>
#include <memory>
#include <print>
#include <dluau.h>
#include <filesystem>
#include <expected>
namespace fs = std::filesystem;
using std::expected, std::unexpected;
using std::string;
fs::path& new_path(lua_State* L, const fs::path& v);
fs::path& check_path(lua_State* L, int idx);
void newindex_parent(lua_State* L, fs::path& p);
void validate_path(lua_State* L, const fs::path& p);
using Path = fs::path;
constexpr const char* fs_namespace{"std.fs"};
struct Symlink_type_info {
    static consteval const char* type_namespace() {return fs_namespace;}
    static consteval const char* type_name() {return "symlink";}
};
struct Dir_type_info {
    static consteval const char* type_namespace() {return fs_namespace;}
    static consteval const char* type_name() {return "directory";}
};
struct File_type_info {
    static consteval const char* type_namespace() {return fs_namespace;}
    static consteval const char* type_name() {return "file";}
};
using T_symlink = Lazy_type<Path, Symlink_type_info>;
using T_directory = Lazy_type<Path, Dir_type_info>;
using T_file = Lazy_type<Path, File_type_info>;

void create_new_file(lua_State* L, const fs::path& path);
fs::copy_options to_copy_options(std::string_view str);

template<class Filesystem_iterator>
int fs_iterator(lua_State* L) {
    Filesystem_iterator& it = *static_cast<Filesystem_iterator*>(lua_touserdata(L, lua_upvalueindex(1)));
    const Filesystem_iterator end;
    while(it != end) {
        bool pushed = true;
        const fs::directory_entry& e = *it;
        auto type = e.is_symlink() ?
            e.symlink_status().type() :
            e.status().type();
        switch (type) {
            case fs::file_type::directory:
                T_directory::make(L, e.path());
            break;
            case fs::file_type::regular:
                T_file::make(L, e.path());
            break;
            case fs::file_type::symlink:
                T_symlink::make(L, e.path());
            break;
            default: pushed = false;
        }
        ++it;
        if (pushed) return 1;
    }
    return 0;
}
