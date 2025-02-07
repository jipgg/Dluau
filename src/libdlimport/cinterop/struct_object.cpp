#include "cinterop.hpp"
using f_info = struct_info::field_info;
constexpr const char* tname{"c_struct_object"};
static const int tag = dluau_registertypetagged(tname);

static struct_object& check_struct_object(lua_State* L, int idx) {
    if (lua_userdatatag(L, idx) != tag) luaL_typeerrorL(L, idx, tname);
    return *static_cast<struct_object*>(lua_touserdatatagged(L, idx, tag));
}
static void push_field(lua_State* L, const f_info& fi, void* data) {
    void* off = static_cast<int8_t*>(data) + fi.memory_offset;
    switch(fi.type) {
        case c_type::c_bool:
            lua_pushboolean(L, *static_cast<bool*>(off));
            break;
        case c_type::c_char:
            dluau_pushc_char(L, *static_cast<char*>(off));
            break;
        case c_type::c_uchar:
            dluau_pushc_uchar(L, *static_cast<unsigned char*>(off));
            break;
        case c_type::c_short:
            dluau_pushc_short(L, *static_cast<short*>(off));
            break;
        case c_type::c_ushort:
            dluau_pushc_ushort(L, *static_cast<unsigned short*>(off));
            break;
        case c_type::c_long:
            dluau_pushc_long(L, *static_cast<long*>(off));
            break;
        case c_type::c_ulong:
            dluau_pushc_ulong(L, *static_cast<unsigned long*>(off));
            break;
        case c_type::c_int:
            dluau_pushc_int(L, *static_cast<int*>(off));
            break;
        case c_type::c_uint:
            dluau_pushc_uint(L, *static_cast<unsigned int*>(off));
            break;
        case c_type::c_float:
            dluau_pushc_float(L, *static_cast<float*>(off));
            break;
        case c_type::c_double:
            lua_pushnumber(L, *static_cast<double*>(off));
            break;
        case c_type::c_void_ptr:
            lua_pushlightuserdata(L, off);
            break;
        case c_type::c_void:
            lua_pushnil(L);
            break;
        case c_type::c_char_ptr:
            lua_pushstring(L, static_cast<const char*>(off));
            break;
        case c_type::c_aggregate:
            lua_pushnil(L);
            break;
    }
}

static int metamethod_index(lua_State* L) {
    auto& obj = check_struct_object(L, 1);
    auto& fields = obj.info->fields;
    const char* key = luaL_checkstring(L, 2);
    if (not fields.contains(key)) luaL_argerrorL(L, 2, "not a field");
    const auto& fi = fields.at(key);
    push_field(L, fi, obj.data);
    return 1;
}
static int newobject(lua_State* L, struct_object&& obj) {
    auto& info = cinterop::check_struct_info(L, 1);
    struct_object* ud = static_cast<struct_object*>(lua_newuserdatatagged(L, sizeof(struct_object), tag));
    new (ud) struct_object(std::move(obj));
    if (luaL_newmetatable(L, tname)) {
        constexpr luaL_Reg lib[] = {
            {"__index", metamethod_index},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, lib);
        lua_pushstring(L, tname);
        lua_setfield(L, -2, "__type");
        lua_setuserdatadtor(L, tag, [](lua_State* L, void* ud) {
            static_cast<struct_object*>(ud)->~struct_object();
        }); 
    }
    lua_setmetatable(L, -2);
    return 1;
}

int cinterop::create_object(lua_State* L) {
    auto& info = cinterop::check_struct_info(L, 1);
    return 1;
}
