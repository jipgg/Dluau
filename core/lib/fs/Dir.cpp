#include <Generic_userdata_template.hpp>
#include "fs.hpp"
#include <filesystem>
#include <algorithm>
#include <ranges>
namespace fs = std::filesystem;
using Directory_meta = Generic_userdata_template<Directory>;
using File_meta = Generic_userdata_template<File>;
static const char* tname = "directory";
template<> const char* Directory_meta::type_name(){return tname;}

static const Directory_meta::Registry namecall = {
    {"getchildren", [](lua_State* L, Directory& d) -> int {
        int i{1};
        lua_newtable(L);
        for (const auto& entry : fs::directory_iterator(d.path)) {
            if (entry.is_directory()) {
                new_directory(L, Directory{.path = entry.path()});
                lua_rawseti(L, -2,  i++);
            } else if (entry.is_regular_file()) {
                new_file(L, {.path = entry.path()});
                lua_rawseti(L, -2, i++);
            }
        }
        return 1;
    }},
};
static const Directory_meta::Registry index = {
    {"parent", [](lua_State* L, Directory& s) -> int {
        new_directory(L, {.path = s.path.parent_path()});
        return 1;
    }},
};
static const Directory_meta::Registry newindex = {
    {"parent", [](lua_State* L, Directory& s) -> int {
        newindex_parent(L, s.path);
        return 0;
    }},
};
static int tostring(lua_State* L) {
    auto& p = check_directory(L, 1);
    lua_pushstring(L, std::format("{}: {{{}}}", tname, p.path.string()).c_str());
    return 1;
}
static int div(lua_State* L) {
    auto& dir = check_directory(L, 1);
    bool is_directory = false;
    fs::path path;
    if (File_meta::is_type(L, 2)) {
        path = check_directory(L, 2).path;
    } else if (File_meta::is_type(L, 2)) {
        path = check_file(L, 2).path;
    } else {
        std::string p = luaL_checkstring(L, 2);
        if (p.ends_with('/')) {
            p.pop_back();
            is_directory = true;
        }
        path = p;
    }
    std::string result = (dir.path / path).string();
    std::ranges::replace(result, '\\', '/');
    if (is_directory) new_directory(L, {.path = result});
    else new_file(L, {.path = result});
    return 1;
}
Directory& new_directory(lua_State* L, const Directory& v) {
    if (not Directory_meta::initialized(L)) {
        const luaL_Reg meta[] = {
            {"__tostring", tostring},
            {nullptr, nullptr}
        };
        lumin_adduserdatatype(tname);
        Directory_meta::init(L, Directory_meta::init_info{
            .index = index,
            .newindex = newindex,
            .namecall = namecall,
            .meta = meta,
        });
    }
    fs::path path = v.path;
    validate_path(L, path);
    standardize(path);
    return Directory_meta::new_udata(L, {.path = std::move(path)});
}
Directory& check_directory(lua_State* L, int idx) {
    return Directory_meta::check_udata(L, idx);
}
