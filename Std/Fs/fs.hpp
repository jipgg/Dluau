#pragma once
#include <gpm.hpp>
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
constexpr const char* fs_namespace{"gpm.fs"};
struct SymlinkTypeInfo {
    static consteval const char* type_namespace() {return fs_namespace;}
    static consteval const char* type_name() {return "Symlink";}
};
struct DirTypeInfo {
    static consteval const char* type_namespace() {return fs_namespace;}
    static consteval const char* type_name() {return "Directory";}
};
struct FileTypeInfo {
    static consteval const char* type_namespace() {return fs_namespace;}
    static consteval const char* type_name() {return "File";}
};
using SymlinkType = LazyUserdataType<Path, SymlinkTypeInfo>;
using DirType = LazyUserdataType<Path, DirTypeInfo>;
using FileType = LazyUserdataType<Path, FileTypeInfo>;

void create_new_file(lua_State* L, const fs::path& path);
fs::copy_options to_copy_options(std::string_view str);

template<class FilesystemIterator>
int fs_iterator(lua_State* L) {
    FilesystemIterator& it = *static_cast<FilesystemIterator*>(lua_touserdata(L, lua_upvalueindex(1)));
    const FilesystemIterator end;
    while(it != end) {
        bool pushed = true;
        const fs::directory_entry& e = *it;
        auto type = e.is_symlink() ?
            e.symlink_status().type() :
            e.status().type();
        switch (type) {
            case fs::file_type::directory:
                DirType::make(L, e.path());
            break;
            case fs::file_type::regular:
                FileType::make(L, e.path());
            break;
            case fs::file_type::symlink:
                SymlinkType::make(L, e.path());
            break;
            default: pushed = false;
        }
        ++it;
        if (pushed) return 1;
    }
    return 0;
}
