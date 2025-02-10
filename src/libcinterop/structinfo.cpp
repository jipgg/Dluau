#include "cinterop.hpp"
constexpr const char* tname{"structinfo"};
static const int tag = dluau_registertypetagged(tname);
using Field_info = Struct_info::Field_info;

static Field_info to_field_info(Lstate L, int idx) {
    lua_rawgetfield(L, idx, "type");
    if (not lua_isstring(L, -1)) luaL_errorL(L, "type must be a string");
    auto type_opt = string_to_param_type(lua_tostring(L, -1));
    if (not type_opt) luaL_errorL(L, "not a c_type '%s'", lua_tostring(L, -1));
    lua_pop(L, 1);
    lua_rawgetfield(L, idx, "memoffset");
    if (not lua_isnumber(L, -1)) luaL_errorL(L, "memory_offset must be a number");
    const int offset = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lua_rawgetfield(L, idx, "arrsize");
    int arr_size{};
    if (lua_isnil(L, -1)) arr_size = 1; 
    else arr_size = lua_tointeger(L, -1);
    lua_pop(L, 1);
    return {
        .type = *type_opt,
        .memory_offset = offset,
        .array_size = arr_size,
    };
}
static void register_fields(Struct_info& si) {
    for (auto& [key, field] : si.fields) {
        dcAggrField(
            si.aggr.get(),
            DCsigchar(field.type),
            field.memory_offset,
            field.array_size
        );
    } 
}
void* Struct_info::newinstance(Lstate L) {
    void* ud = lua_newuserdata(L, memory_size);
    memset(ud, 0, memory_size);
    if (metatable != nullptr) {
        lua_getref(L, *metatable);
        lua_setmetatable(L, -2);
    }
    return ud;
}

static void set_field(Lstate L, const Field_info& fi, void* data, int idx, int n = 0) {
    void* off = static_cast<int8_t*>(data) + fi.memory_offset;
    if (n >= fi.array_size) luaL_errorL(L, "out of range");
    switch(fi.type) {
        case C_type::c_bool:
            *(static_cast<bool*>(off) + n) = luaL_checkboolean(L, idx);
            break;
        case C_type::c_char:
            *(static_cast<char*>(off) + n) = dluauC_checkchar(L, idx);
            break;
        case C_type::c_uchar:
            *(static_cast<unsigned char*>(off) + n) = dluauC_checkuchar(L, idx);
            break;
        case C_type::c_short:
            *(static_cast<short*>(off) + n) = dluauC_checkshort(L, idx);
            break;
        case C_type::c_ushort:
            *(static_cast<unsigned short*>(off) + n) = dluauC_checkushort(L, idx);
            break;
        case C_type::c_long:
            *(static_cast<long*>(off) + n) = dluauC_checklong(L, idx);
            break;
        case C_type::c_ulong:
            *(static_cast<unsigned long*>(off) + n) = dluauC_checkulong(L, idx);
            break;
        case C_type::c_int:
            *(static_cast<int*>(off) + n) = dluauC_checkint(L, idx);
            break;
        case C_type::c_uint:
            *(static_cast<unsigned int*>(off) + n) = dluauC_checkuint(L, idx);
            break;
        case C_type::c_float:
            *(static_cast<float*>(off) + n) = dluauC_checkfloat(L, idx);
            break;
        case C_type::c_double:
            *(static_cast<double*>(off) + n) = luaL_checknumber(L, idx);
            break;
        case C_type::c_void_ptr:
            *(static_cast<void**>(off) + n) = lua_tolightuserdata(L, idx);
            break;
        case C_type::c_void:
            luaL_errorL(L, "cannot set void");
            break;
        case C_type::c_string:
            luaL_errorL(L, "cannot set string");
            break;
    }
}

static void push_field(Lstate L, const Field_info& fi, void* data, int n = 0) {
    void* off = static_cast<int8_t*>(data) + fi.memory_offset;
    if (n >= fi.array_size) luaL_errorL(L, "out of range");
    switch(fi.type) {
        case C_type::c_bool:
            lua_pushboolean(L, *(static_cast<bool*>(off) + n));
            break;
        case C_type::c_char:
            dluauC_pushchar(L, *(static_cast<char*>(off) + n));
            break;
        case C_type::c_uchar:
            dluauC_pushuchar(L, *(static_cast<unsigned char*>(off) + n));
            break;
        case C_type::c_short:
            dluauC_pushshort(L, *(static_cast<short*>(off) + n));
            break;
        case C_type::c_ushort:
            dluauC_pushushort(L, *(static_cast<unsigned short*>(off) + n));
            break;
        case C_type::c_long:
            dluauC_pushlong(L, *(static_cast<long*>(off) + n));
            break;
        case C_type::c_ulong:
            dluauC_pushulong(L, *(static_cast<unsigned long*>(off) + n));
            break;
        case C_type::c_int:
            dluauC_pushint(L, *(static_cast<int*>(off) + n));
            break;
        case C_type::c_uint:
            dluauC_pushuint(L, *(static_cast<unsigned int*>(off) + n));
            break;
        case C_type::c_float:
            dluauC_pushfloat(L, *(static_cast<float*>(off) + n));
            break;
        case C_type::c_double:
            lua_pushnumber(L, *(static_cast<double*>(off) + n));
            break;
        case C_type::c_void_ptr:
            lua_pushlightuserdata(L, *(static_cast<void**>(off) + n));
            break;
        case C_type::c_void:
            lua_pushnil(L);
            break;
        case C_type::c_string:
            lua_pushstring(L, *(static_cast<const char**>(off) + n));
            break;
    }
}

static void lua_newstructinfo(Lstate L, Shared<Struct_info>&& si) {
    using Sh_si = Shared<Struct_info>;
    Sh_si* ud = static_cast<Sh_si*>(lua_newuserdatatagged(L, sizeof(Sh_si), tag));
    new (ud) Sh_si(std::move(si));
    if (luaL_newmetatable(L, tname)) {
        lua_pushstring(L, tname);
        lua_setfield(L, -2, "__type");
        lua_setuserdatadtor(L, tag, [](lua_State* L, void* data) {
            static_cast<Sh_si*>(data)->~Sh_si();
        });
    }
    lua_setmetatable(L, -2);
}

Shared<Struct_info>& cinterop::check_struct_info(Lstate L, int idx) {
    if (lua_userdatatag(L, idx) != tag) luaL_typeerrorL(L, idx, tname);
    return *static_cast<Shared<Struct_info>*>(lua_touserdatatagged(L, idx, tag));
}
bool cinterop::is_struct_info(Lstate L, int idx) {
    return lua_userdatatag(L, idx) == tag;
}
std::shared_ptr<Struct_info>& cinterop::to_struct_info(Lstate L, int idx) {
    return *static_cast<Shared<Struct_info>*>(lua_touserdatatagged(L, idx, tag));
}
int cinterop::get_struct_field(lua_State* L) {
    auto& info = cinterop::check_struct_info(L, 1);
    void* data = lua_touserdata(L, 2);
    const char* key = luaL_checkstring(L, 3);
    const int arrindex = luaL_optinteger(L, 4, 0);
    if (not info->fields.contains(key)) luaL_errorL(L, "invalid field key");
    push_field(L, info->fields.at(key), data, arrindex);
    return 1;
}
int cinterop::set_struct_field(lua_State* L) {
    auto& info = cinterop::check_struct_info(L, 1);
    void* data = lua_touserdata(L, 2);
    const char* key = luaL_checkstring(L, 3);
    const int arrindex = luaL_optinteger(L, 5, 0);
    if (not info->fields.contains(key)) luaL_errorL(L, "invalid field key");
    set_field(L, info->fields[key], data, 4, arrindex);
    return 0;
}
int cinterop::new_struct_instance(lua_State* L) {
    auto& info = cinterop::check_struct_info(L, 1);
    info->newinstance(L);
    return 1;
}

int cinterop::create_struct_info(lua_State* L) {
    const int memory_size = luaL_checkinteger(L, 1);
    if (not lua_istable(L, 2)) luaL_typeerrorL(L, 2, "table");
    int table_index = lua_absindex(L, 2);
    Flat_map<String, Field_info> fields;
    lua_pushnil(L);
    while (lua_next(L, table_index) != 0) {
        String key = luaL_checkstring(L, -2);
        auto info = to_field_info(L, -1);
        fields.emplace(std::move(key), std::move(info));
        lua_pop(L, 1);
    }
    const int max_field_count = fields.size();
    Unique<int, Func<void(int*)>> metatable{nullptr, [M = lua_mainthread(L)](int* ref) {
        lua_unref(M, *ref);
    }};
    if (lua_istable(L, 3)) {
        metatable.reset(new int(lua_ref(L, 3)));
    }
    Shared<Struct_info> si = std::make_shared<Struct_info>(Struct_info{
        .memory_size = memory_size,
        .fields = std::move(fields),
        .aggr{dcNewAggr(max_field_count, memory_size), dcFreeAggr},
        .metatable = std::move(metatable),
    });
    register_fields(*si);
    dcCloseAggr(si->aggr.get());
    lua_newstructinfo(L, std::move(si));
    return 1;
};
