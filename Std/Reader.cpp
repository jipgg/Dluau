#include "std.hpp"
#include <lua_utility.hpp>
static const char* tname = "gpm.Reader";
using namespace gpm;
using std::string;

static int reader_line_iterator(lua_State* L) {
    auto& reader = Reader_type::check(L, lua_upvalueindex(1));
    char delimiter = lua_tointeger(L, lua_upvalueindex(2));
    string line;
    if (std::getline(*reader, line, delimiter)) {
        lu::push(L, line);
        return 1;
    }
    return 0;
}
static const Reader_type::Registry namecall = {
    {"free", [](lua_State* L, Reader& r) -> int {
        r.reset();
        return 0;
    }},
    {"get_line", [](lua_State* L, Reader& r) -> int {
        char delim = *luaL_optstring(L, 2, "\n");
        string line;
        if (not std::getline(*r, line, delim)) return 0;
        lu::push(L, line);
        return 1;
    }},
    {"each_line", [](lua_State* L, Reader& r) -> int {
        char delim = *luaL_optstring(L, 2, "\n");
        lua_pushvalue(L, 1);
        lu::push(L, static_cast<int>(delim));
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
        string all;
        string line;
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

static const Reader_type::Registry index = {
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
static const Reader_type::Registry newindex = {
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

template<> const Reader_type::Init_info Reader_type::init_info {
    .index = index,
    .newindex = newindex,
    .namecall = namecall,
    .checker = [](lua_State* L, Reader& self) -> int {
        if (self == nullptr) lu::error(L, "{} resource was already freed", tname);
        return 0;
    },
};
