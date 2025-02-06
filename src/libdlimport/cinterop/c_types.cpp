#include "cinterop.hpp"
#include <dluau.h>
#include <string>
using uint = unsigned int;
using ulong = unsigned long;
using ushort = unsigned short;
using uchar = unsigned char;
constexpr const char* c_int = "c_int";
constexpr const char* c_uint = "c_uint";
constexpr const char* c_short = "c_short";
constexpr const char* c_ushort = "c_ushort";
constexpr const char* c_long = "c_long";
constexpr const char* c_ulong = "c_ulong";
constexpr const char* c_char = "c_char";
constexpr const char* c_uchar = "c_uchar";
constexpr const char* c_float = "c_float";

template <class T>
int get_tag(const char* tname) {
    static const int tag = dluau_registertypetagged(tname);
    return tag;
}
template <class T>
T to(const char* tname, lua_State* L, int idx) {
    return *static_cast<T*>(lua_touserdatatagged(L, idx, get_tag<T>(tname)));
}
template <class T>
T check(const char* tname, lua_State* L, int idx) {
    const int tag = get_tag<T>(tname);
    if (lua_userdatatag(L, idx) != tag) luaL_typeerrorL(L, idx, tname);
    return *static_cast<T*>(lua_touserdatatagged(L, idx, tag));
}
template <class T>
void create(lua_State* L, T v, const char* tname) {
    static const int tag = get_tag<T>(tname);
    T* ud = static_cast<T*>(lua_newuserdatatagged(L, sizeof(T), tag));
    *ud = v;
    if (luaL_newmetatable(L, tname)) {
        constexpr luaL_Reg meta[] = {
            {"__tostring", [](lua_State* L) -> int {
                T value = *static_cast<T*>(lua_touserdatatagged(L, 1, tag));
                lua_pushstring(L, std::to_string(value).c_str());
                return 1;
            }},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, tname);
        lua_setfield(L, -2, "__type");
    }
    lua_setmetatable(L, -2);
}

void dluau_pushc_int(lua_State* L, int v) {
    create(L, v, c_int);
}
void dluau_pushc_uint(lua_State* L, unsigned int v) {
    create(L, v, c_uint);
}
void dluau_pushc_short(lua_State* L, short v) {
    create(L, v, c_short);
}
void dluau_pushc_ushort(lua_State* L, unsigned short v) {
    create(L, v, c_ushort);
}
void dluau_pushc_long(lua_State* L, long v) {
    create(L, v, c_long);
}
void dluau_pushc_ulong(lua_State* L, unsigned long v) {
    create(L, v, c_ulong);
}
void dluau_pushc_char(lua_State* L, char v) {
    create(L, v, c_char);
}
void dluau_pushc_uchar(lua_State* L, unsigned char v) {
    create(L, v, c_uchar);
}
void dluau_pushc_float(lua_State* L, float v) {
    create(L, v, c_float);
}

int dluau_toc_int(lua_State* L, int idx) {
    return to<int>(c_int, L, idx);
}
unsigned int dluau_toc_uint(lua_State* L, int idx) {
    return to<uint>(c_uint, L, idx);
}
short dluau_toc_short(lua_State* L, int idx) {
    return to<short>(c_short, L, idx);
}
unsigned short dluau_toc_ushort(lua_State* L, int idx) {
    return to<ushort>(c_ushort, L, idx);
}
long dluau_toc_long(lua_State* L, int idx) {
    return to<long>(c_long, L, idx);
}
unsigned long dluau_toc_ulong(lua_State* L, int idx) {
    return to<ulong>(c_ulong, L, idx);
}
char dluau_toc_char(lua_State* L, int idx) {
    return to<char>(c_char, L, idx);
}
unsigned char dluau_toc_uchar(lua_State* L, int idx) {
    return to<uchar>(c_uchar, L, idx);
}
float dluau_toc_float(lua_State* L, int idx) {
    return to<float>(c_float, L, idx);
}

int dluau_checkc_int(lua_State* L, int idx) {
    return check<int>(c_int, L, idx);
}
unsigned int dluau_checkc_uint(lua_State* L, int idx) {
    return check<uint>(c_uint, L, idx);
}
short dluau_checkc_short(lua_State* L, int idx) {
    return check<short>(c_short, L, idx);

}
unsigned short dluau_checkc_ushort(lua_State* L, int idx) {
    return check<ushort>(c_ushort, L, idx);

}
long dluau_checkc_long(lua_State* L, int idx) {
    return check<long>(c_long, L, idx);
}
unsigned long dluau_checkc_ulong(lua_State* L, int idx) {
    return check<ulong>(c_ulong, L, idx);
}
char dluau_checkc_char(lua_State* L, int idx) {
    return check<char>(c_char, L, idx);
}
unsigned char dluau_checkc_uchar(lua_State* L, int idx) {
    return check<uchar>(c_uchar, L, idx);
}
float dluau_checkc_float(lua_State* L, int idx) {
    return check<float>(c_float, L, idx);
}

void cinterop::push_c_types(lua_State *L) {
    // pre-register type tags
    get_tag<int>(c_int);
    get_tag<uint>(c_uint);
    get_tag<short>(c_short);
    get_tag<ushort>(c_ushort);
    get_tag<long>(c_long);
    get_tag<ulong>(c_ulong);
    get_tag<char>(c_char);
    get_tag<uchar>(c_uchar);
    get_tag<float>(c_float);
    constexpr luaL_Reg ctors[] = {
        {"int", [](lua_State* L) -> int{
            const int v = lua_isstring(L, 1) ? std::stoi(lua_tostring(L, 1)) : luaL_checkinteger(L, 1);
            if (lua_isstring(L, 1)) dluau_pushc_int(L, std::stoi(lua_tostring(L, 1)));
            else if (lua_isnumber(L, 1)) dluau_pushc_int(L, lua_tointeger(L, 1));
            else luaL_typeerrorL(L, 1, "string | number");
            return 1;
        }},
        {"uint", [](lua_State* L) -> int {
            try {
            if (lua_isstring(L, 1)) dluau_pushc_uint(L, std::stoul(lua_tostring(L, 1)));
            else if (lua_isnumber(L, 1)) dluau_pushc_uint(L, lua_tointeger(L, 1));
            else luaL_typeerrorL(L, 1, "string | number");
            return 1;
            } catch (std::exception& e) {luaL_errorL(L, e.what());}
        }},
        {"short", [](lua_State* L) -> int {
            if (lua_isstring(L, 1)) dluau_pushc_short(L, std::stoi(lua_tostring(L, 1)));
            else if (lua_isnumber(L, 1)) dluau_pushc_short(L, lua_tointeger(L, 1));
            else luaL_typeerrorL(L, 1, "string | number");
            return 1;
        }},
        {"ushort", [](lua_State* L) -> int {
            if (lua_isstring(L, 1)) dluau_pushc_ushort(L, std::stoul(lua_tostring(L, 1)));
            else if (lua_isnumber(L, 1)) dluau_pushc_ushort(L, static_cast<ushort>(lua_tointeger(L, 1)));
            else luaL_typeerrorL(L, 1, "string | number");
            return 1;
        }},
        {"long", [](lua_State* L) -> int {
            if (lua_isstring(L, 1)) dluau_pushc_long(L, std::stol(lua_tostring(L, 1)));
            else if (lua_isnumber(L, 1)) dluau_pushc_long(L, lua_tointeger(L, 1));
            else luaL_typeerrorL(L, 1, "string | number");
            return 1;
        }},
        {"ulong", [](lua_State* L) -> int {
            if (lua_isstring(L, 1)) dluau_pushc_ulong(L, std::stoul(lua_tostring(L, 1)));
            else if (lua_isnumber(L, 1)) dluau_pushc_ulong(L, lua_tointeger(L, 1));
            else luaL_typeerrorL(L, 1, "string | number");
            return 1;
        }},
        {"char", [](lua_State* L) -> int {
            if (lua_isstring(L, 1)) dluau_pushc_char(L, *lua_tostring(L, 1));
            else if (lua_isnumber(L, 1)) dluau_pushc_char(L, static_cast<char>(lua_tointeger(L, 1)));
            else luaL_typeerrorL(L, 1, "string | number");
            return 1;
        }},
        {"uchar", [](lua_State* L) -> int {
            if (lua_isstring(L, 1)) dluau_pushc_uchar(L, *lua_tostring(L, 1));
            else if (lua_isnumber(L, 1)) dluau_pushc_uchar(L, static_cast<uchar>(lua_tointeger(L, 1)));
            else luaL_typeerrorL(L, 1, "string | number");
            return 1;
        }},
        {"float", [](lua_State* L) -> int {
            dluau_pushc_float(L, luaL_checknumber(L, 1));
            return 1;
        }},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, ctors);
}
