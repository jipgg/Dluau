#include <userdata_template.hpp>
#include "fs.hpp"
#include <filesystem>
#include <algorithm>
namespace fs = std::filesystem;
using Reader_ut = Userdata_template<Reader>;
static const char* tname = "reader";
template<> const char* Reader_ut::type_name(){return tname;}

int reader_line_iterator(lua_State* L) {
    auto& reader = Reader_ut::check_udata(L, lua_upvalueindex(1));
    std::string line;
    if (std::getline(*reader, line)) {
        lua_pushstring(L, line.c_str());
        return 1;
    }
    return 0;
}
static int ranged_line_iterator(lua_State* L) {
    auto& reader = Reader_ut::check_udata(L, lua_upvalueindex(1));
    int count = lua_tointeger(L, lua_upvalueindex(2));
    lua_pushinteger(L, --count);
    lua_replace(L, lua_upvalueindex(2));
    if (count < 0) return 0;
    std::string line;
    if (std::getline(*reader, line)) {
        lua_pushstring(L, line.c_str());
        return 1;
    }
    return 0;
}

static const Reader_ut::Registry namecall = {
    {"getline", [](lua_State* L, Reader& r) -> int {
        std::string line;
        if (not std::getline(*r, line)) return 0;
        lua_pushstring(L, line.c_str());
        return 1;
    }},
    {"eachline", [](lua_State* L, Reader& r) -> int {
        lua_pushvalue(L, 1);
        lua_pushcclosure(L, reader_line_iterator, "line_iterator", 1);
        return 1;
    }},
};

static int concat(lua_State* L) {
    auto& reader = check_reader(L, 1);
    if (lua_isnoneornil(L, 2)) {
        lua_pushvalue(L, 1);
        lua_pushcclosure(L, reader_line_iterator, "line_iterator", 1);
        return 1;
    } else if (lua_isnumber(L, 2)) {
        lua_pushvalue(L, 1);
        lua_pushvalue(L, 2);
        lua_pushcclosure(L, ranged_line_iterator, "ranged_line_iterator", 2);
        return 1;
    }
    luaL_typeerrorL(L, 2, "nil or number");
}

static const Reader_ut::Registry index = {
    {"eof", [](lua_State* L, Reader& r) -> int {
        lua_pushboolean(L, r->eof());
        return 1;
    }},
};
Reader& new_reader(lua_State* L, Reader&& v) {
    if (not Reader_ut::initialized(L)) {
        const luaL_Reg meta[] = {
            {"__concat", concat},
            {nullptr, nullptr}
        };
        lumin_adduserdatatype(tname);
        Reader_ut::init(L, Reader_ut::init_info{
            .index = index,
            .namecall = namecall,
            .meta = meta,
        });
    }
    return Reader_ut::new_udata(L, std::move(v));
}
Reader& check_reader(lua_State* L, int idx) {
    return Reader_ut::check_udata(L, idx);
}
