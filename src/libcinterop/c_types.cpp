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
static T resolve_type(lua_State* L, int idx, int tag) {
    if (lua_isnumber(L, idx)) {
        return static_cast<T>(lua_tonumber(L, idx));
    } else if (lua_userdatatag(L, idx) == tag) {
        return *static_cast<T*>(lua_touserdatatagged(L, idx, tag));
    } else luaL_argerrorL(L, idx, "invalid type");
}

template <class T>
void create(lua_State* L, T v, const char* tname) {
    static const char* s_tname = tname;
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
            {"__add", [](lua_State* L) -> int {
                T a = resolve_type<T>(L, 1, tag);
                T b = resolve_type<T>(L, 2, tag);
                create(L, a + b, s_tname);
                return 1;
            }},
            {"__sub", [](lua_State* L) -> int {
                T a = resolve_type<T>(L, 1, tag);
                T b = resolve_type<T>(L, 2, tag);
                create(L, a - b, s_tname);
                return 1;
            }},
            {"__div", [](lua_State* L) -> int {
                T a = resolve_type<T>(L, 1, tag);
                T b = resolve_type<T>(L, 2, tag);
                create(L, a / b, s_tname);
                return 1;
            }},
            {"__idiv", [](lua_State* L) -> int {
                T a = resolve_type<T>(L, 1, tag);
                T b = resolve_type<T>(L, 2, tag);
                create(L, int(a) / int(b), s_tname);
                return 1;
            }},
            {"__mul", [](lua_State* L) -> int {
                T a = resolve_type<T>(L, 1, tag);
                T b = resolve_type<T>(L, 2, tag);
                create(L, a * b, s_tname);
                return 1;
            }},
            {"__pow", [](lua_State* L) -> int {
                T a = resolve_type<T>(L, 1, tag);
                T b = resolve_type<T>(L, 2, tag);
                create(L, static_cast<T>(std::pow(double(a), double(b))), s_tname);
                return 1;
            }},
            {"__mod", [](lua_State* L) -> int {
                T a = resolve_type<T>(L, 1, tag);
                T b = resolve_type<T>(L, 2, tag);
                create(L, int(a) % int(b), s_tname);
                return 1;
            }},
            {"__eq", [](lua_State* L) -> int {
                T a = resolve_type<T>(L, 1, tag);
                T b = resolve_type<T>(L, 2, tag);
                lua_pushboolean(L, a == b);
                return 1;
            }},
            {"__lt", [](lua_State* L) -> int {
                T a = resolve_type<T>(L, 1, tag);
                T b = resolve_type<T>(L, 2, tag);
                lua_pushboolean(L, a < b);
                return 1;
            }},
            {"__le", [](lua_State* L) -> int {
                T a = resolve_type<T>(L, 1, tag);
                T b = resolve_type<T>(L, 2, tag);
                lua_pushboolean(L, a <= b);
                return 1;
            }},
            {"__unm", [](lua_State* L) -> int {
                T a = resolve_type<T>(L, 1, tag);
                create(L, -a, s_tname);
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

void dluauC_pushint(lua_State* L, int v) {
    create(L, v, c_int);
}
void dluauC_pushuint(lua_State* L, unsigned int v) {
    create(L, v, c_uint);
}
void dluauC_pushshort(lua_State* L, short v) {
    create(L, v, c_short);
}
void dluauC_pushushort(lua_State* L, unsigned short v) {
    create(L, v, c_ushort);
}
void dluauC_pushlong(lua_State* L, long v) {
    create(L, v, c_long);
}
void dluauC_pushulong(lua_State* L, unsigned long v) {
    create(L, v, c_ulong);
}
void dluauC_pushchar(lua_State* L, char v) {
    create(L, v, c_char);
}
void dluauC_pushuchar(lua_State* L, unsigned char v) {
    create(L, v, c_uchar);
}
void dluauC_pushfloat(lua_State* L, float v) {
    create(L, v, c_float);
}

int dluauC_toint(lua_State* L, int idx) {
    return to<int>(c_int, L, idx);
}
unsigned int dluauC_touint(lua_State* L, int idx) {
    return to<uint>(c_uint, L, idx);
}
short dluauC_toshort(lua_State* L, int idx) {
    return to<short>(c_short, L, idx);
}
unsigned short dluauC_toushort(lua_State* L, int idx) {
    return to<ushort>(c_ushort, L, idx);
}
long dluauC_tolong(lua_State* L, int idx) {
    return to<long>(c_long, L, idx);
}
unsigned long dluauC_toulong(lua_State* L, int idx) {
    return to<ulong>(c_ulong, L, idx);
}
char dluauC_tochar(lua_State* L, int idx) {
    return to<char>(c_char, L, idx);
}
unsigned char dluauC_touchar(lua_State* L, int idx) {
    return to<uchar>(c_uchar, L, idx);
}
float dluauC_tofloat(lua_State* L, int idx) {
    return to<float>(c_float, L, idx);
}

int dluauC_checkint(lua_State* L, int idx) {
    return check<int>(c_int, L, idx);
}
unsigned int dluauC_checkuint(lua_State* L, int idx) {
    return check<uint>(c_uint, L, idx);
}
short dluauC_checkshort(lua_State* L, int idx) {
    return check<short>(c_short, L, idx);

}
unsigned short dluauC_checkushort(lua_State* L, int idx) {
    return check<ushort>(c_ushort, L, idx);

}
long dluauC_checklong(lua_State* L, int idx) {
    return check<long>(c_long, L, idx);
}
unsigned long dluauC_checkulong(lua_State* L, int idx) {
    return check<ulong>(c_ulong, L, idx);
}
char dluauC_checkchar(lua_State* L, int idx) {
    return check<char>(c_char, L, idx);
}
unsigned char dluauC_checkuchar(lua_State* L, int idx) {
    return check<uchar>(c_uchar, L, idx);
}
float dluauC_checkfloat(lua_State* L, int idx) {
    return check<float>(c_float, L, idx);
}

int cinterop::c_type_sizeof(lua_State *L) {
    auto type_opt = string_to_param_type(luaL_checkstring(L, 1));
    if (not type_opt) luaL_argerrorL(L, 1, "not a c_type");
    switch(*type_opt) {
        case c_type::c_float:
            lua_pushinteger(L, sizeof(float));
            return 1;
        case c_type::c_char:
        case c_type::c_uchar:
        case c_type::c_bool:
            lua_pushinteger(L, sizeof(char));
            return 1;
        case c_type::c_int:
        case c_type::c_uint:
            lua_pushinteger(L, sizeof(int));
            return 1;
        case c_type::c_short:
        case c_type::c_ushort:
            lua_pushinteger(L, sizeof(short));
            return 1;
        case c_type::c_long:
        case c_type::c_ulong:
            lua_pushinteger(L, sizeof(long));
            return 1;
        case c_type::c_double:
            lua_pushinteger(L, sizeof(double));
            return 1;
        case c_type::c_void_ptr:
        case c_type::c_string:
            lua_pushinteger(L, sizeof(void*));
            return 1;
        default:
            break;
    }
    luaL_argerrorL(L, 1, "not a c_type");
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
            if (lua_isstring(L, 1)) dluauC_pushint(L, std::stoi(lua_tostring(L, 1)));
            else if (lua_isnumber(L, 1)) dluauC_pushint(L, lua_tointeger(L, 1));
            else luaL_typeerrorL(L, 1, "string | number");
            return 1;
        }},
        {"uint", [](lua_State* L) -> int {
            try {
            if (lua_isstring(L, 1)) dluauC_pushuint(L, std::stoul(lua_tostring(L, 1)));
            else if (lua_isnumber(L, 1)) dluauC_pushuint(L, lua_tointeger(L, 1));
            else luaL_typeerrorL(L, 1, "string | number");
            return 1;
            } catch (std::exception& e) {luaL_errorL(L, e.what());}
        }},
        {"short", [](lua_State* L) -> int {
            if (lua_isstring(L, 1)) dluauC_pushshort(L, std::stoi(lua_tostring(L, 1)));
            else if (lua_isnumber(L, 1)) dluauC_pushshort(L, lua_tointeger(L, 1));
            else luaL_typeerrorL(L, 1, "string | number");
            return 1;
        }},
        {"ushort", [](lua_State* L) -> int {
            if (lua_isstring(L, 1)) dluauC_pushushort(L, std::stoul(lua_tostring(L, 1)));
            else if (lua_isnumber(L, 1)) dluauC_pushushort(L, static_cast<ushort>(lua_tointeger(L, 1)));
            else luaL_typeerrorL(L, 1, "string | number");
            return 1;
        }},
        {"long", [](lua_State* L) -> int {
            if (lua_isstring(L, 1)) dluauC_pushlong(L, std::stol(lua_tostring(L, 1)));
            else if (lua_isnumber(L, 1)) dluauC_pushlong(L, lua_tointeger(L, 1));
            else luaL_typeerrorL(L, 1, "string | number");
            return 1;
        }},
        {"ulong", [](lua_State* L) -> int {
            if (lua_isstring(L, 1)) dluauC_pushulong(L, std::stoul(lua_tostring(L, 1)));
            else if (lua_isnumber(L, 1)) dluauC_pushulong(L, lua_tointeger(L, 1));
            else luaL_typeerrorL(L, 1, "string | number");
            return 1;
        }},
        {"char", [](lua_State* L) -> int {
            if (lua_isstring(L, 1)) dluauC_pushchar(L, *lua_tostring(L, 1));
            else if (lua_isnumber(L, 1)) dluauC_pushchar(L, static_cast<char>(lua_tointeger(L, 1)));
            else luaL_typeerrorL(L, 1, "string | number");
            return 1;
        }},
        {"uchar", [](lua_State* L) -> int {
            if (lua_isstring(L, 1)) dluauC_pushuchar(L, *lua_tostring(L, 1));
            else if (lua_isnumber(L, 1)) dluauC_pushuchar(L, static_cast<uchar>(lua_tointeger(L, 1)));
            else luaL_typeerrorL(L, 1, "string | number");
            return 1;
        }},
        {"float", [](lua_State* L) -> int {
            dluauC_pushfloat(L, luaL_checknumber(L, 1));
            return 1;
        }},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, ctors);
}
