#include <userdata_template.hpp>
#include "lib.hpp"
using Reader_ut = Userdata_template<libio::Reader>;
static const char* tname = "ioReader";
template<> const char* Reader_ut::type_name(){return tname;}

namespace libio {
int reader_line_iterator(lua_State* L) {
    auto& reader = Reader_ut::check_udata(L, lua_upvalueindex(1));
    char delimiter = lua_tointeger(L, lua_upvalueindex(2));
    std::string line;
    if (std::getline(*reader, line, delimiter)) {
        lua_pushstring(L, line.c_str());
        return 1;
    }
    return 0;
}
static const Reader_ut::Registry namecall = {
    {"get_line", [](lua_State* L, Reader& r) -> int {
        char delim = *luaL_optstring(L, 2, "\n");
        std::string line;
        if (not std::getline(*r, line, delim)) return 0;
        lua_pushstring(L, line.c_str());
        return 1;
    }},
    {"line_iterator", [](lua_State* L, Reader& r) -> int {
        char delim = *luaL_optstring(L, 2, "\n");
        lua_pushvalue(L, 1);
        lua_pushinteger(L, delim);
        lua_pushcclosure(L, reader_line_iterator, "line_iterator", 2);
        return 1;
    }},
    {"read", [](lua_State* L, Reader& r) -> int {
        size_t buf_len;
        char* buf = static_cast<char*>(luaL_checkbuffer(L, 2, &buf_len));
        r->read(buf, buf_len);
        lua_pushinteger(L, r->gcount());
        lua_pushvalue(L, 2);
        return 2;
    }},
    {"read_all", [](lua_State* L, Reader& r) -> int {
        std::string all;
        std::string line;
        auto prev_pos = r->tellg();
        r->seekg(0, std::ios::beg);
        while (std::getline(*r, line)) all += line + "\n";
        r->clear();
        r->seekg(prev_pos);
        char* buf = static_cast<char*>(lua_newbuffer(L, all.size()));
        std::memcpy(buf, all.data(), all.size());
        return 1;
    }},
    {"get", [](lua_State* L, Reader& r) -> int {
        lua_pushinteger(L, r->get());
        return 1;
    }},
    {"peek", [](lua_State* L, Reader& r) -> int {
        lua_pushinteger(L, r->peek());
        return 1;
    }},
    {"ignore", [](lua_State* L, Reader& r) -> int {
        r->ignore(luaL_optinteger(L, 2, 1));
        return 0;
    }},
    {"reset", [](lua_State* L, Reader& r) -> int {
        r->seekg(0, std::ios::beg);
        r->clear();
        return 0;
    }},
    {"try_offset", [](lua_State* L, Reader& r) -> int {
        auto old_pos = r->tellg();
        bool success = true;
        r->seekg(luaL_checkinteger(L, 2), std::ios::beg);
        if (r->fail()) {
            r->seekg(old_pos);
            r->clear();
            success = false;
        }
        lua_pushboolean(L, success);
        return 1;
    }},
};

static const Reader_ut::Registry index = {
    {"eof", [](lua_State* L, Reader& r) -> int {
        lua_pushboolean(L, r->eof());
        return 1;
    }},
    {"fail", [](lua_State* L, Reader& r) -> int {
        lua_pushboolean(L, r->fail());
        return 1;
    }},
    {"good", [](lua_State* L, Reader& r) -> int {
        lua_pushboolean(L, r->good());
        return 1;
    }},
    {"bad", [](lua_State* L, Reader& r) -> int {
        lua_pushboolean(L, r->good());
        return 1;
    }},
    {"offset", [](lua_State* L, Reader& r) -> int {
        lua_pushinteger(L, r->tellg());
        return 1;
    }},
    {"last_bytes_count", [](lua_State* L, Reader& r) -> int {
        r->unget();
        lua_pushinteger(L, r->gcount());
        return 1;
    }},
};
static const Reader_ut::Registry newindex = {
    {"offset", [](lua_State* L, Reader& r) -> int {
        auto old_pos = r->tellg();
        r->seekg(luaL_checkinteger(L, 3), std::ios::beg);
        if (r->fail()) {
            r->seekg(old_pos);
            r->clear(std::ios::failbit);
            luaL_errorL(L, "offset out of range.");
        }
        return 0;
    }},
};

Reader& new_reader(lua_State* L, Reader&& v) {
    if (not Reader_ut::initialized(L)) {
        lumin_adduserdatatype(tname);
        Reader_ut::init(L, Reader_ut::init_info{
            .index = index,
            .namecall = namecall,
        });
    }
    return Reader_ut::new_udata(L, std::move(v));
}
Reader& check_reader(lua_State* L, int idx) {
    return Reader_ut::check_udata(L, idx);
}
}
