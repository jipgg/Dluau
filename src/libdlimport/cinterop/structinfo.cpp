#include "cinterop.hpp"
constexpr const char* tname{"c_structinfo"};
static const int tag = dluau_registertypetagged(tname);
namespace bc = boost::container;

static struct_info::field_info to_field_info(lua_State* L, int idx) {
    lua_rawgetfield(L, idx, "type");
    if (not lua_isstring(L, -1)) luaL_errorL(L, "type must be a string");
    auto type_opt = string_to_param_type(lua_tostring(L, -1));
    if (not type_opt) luaL_errorL(L, "not a c_type");
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
static void register_fields(struct_info& si) {
    for (auto& [key, field] : si.fields) {
        dcAggrField(
            si.aggr.get(),
            DCsigchar(field.type),
            field.memory_offset,
            field.array_size
        );
    } 
}

static void set_field(lua_State* L, const struct_info::field_info& fi, void* data, int idx) {
    void* off = static_cast<int8_t*>(data) + fi.memory_offset;
    switch(fi.type) {
        case c_type::c_bool:
            *static_cast<bool*>(off) = luaL_checkboolean(L, idx);
            break;
        case c_type::c_char:
            *static_cast<char*>(off) = dluau_checkc_char(L, idx);
            break;
        case c_type::c_uchar:
            *static_cast<unsigned char*>(off) = dluau_checkc_uchar(L, idx);
            break;
        case c_type::c_short:
            *static_cast<short*>(off) = dluau_checkc_short(L, idx);
            break;
        case c_type::c_ushort:
            *static_cast<unsigned short*>(off) = dluau_checkc_ushort(L, idx);
            break;
        case c_type::c_long:
            *static_cast<long*>(off) = dluau_checkc_long(L, idx);
            break;
        case c_type::c_ulong:
            *static_cast<unsigned long*>(off) = dluau_checkc_ulong(L, idx);
            break;
        case c_type::c_int:
            *static_cast<int*>(off) = dluau_checkc_int(L, idx);
            break;
        case c_type::c_uint:
            *static_cast<unsigned int*>(off) = dluau_checkc_uint(L, idx);
            break;
        case c_type::c_float:
            *static_cast<float*>(off) = dluau_checkc_float(L, idx);
            break;
        case c_type::c_double:
            *static_cast<double*>(off) = luaL_checknumber(L, idx);
            break;
        case c_type::c_void_ptr:
            *static_cast<void**>(off) = lua_tolightuserdata(L, idx);
            break;
        case c_type::c_void:
            luaL_errorL(L, "cannot set void");
            break;
        case c_type::c_char_ptr:
            luaL_errorL(L, "cannot set string");
            break;
        case c_type::c_aggregate:
            luaL_errorL(L, "cannot set aggr");
            break;
    }
}

static void push_field(lua_State* L, const struct_info::field_info& fi, void* data) {
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

static void lua_newstructinfo(lua_State* L, std::shared_ptr<struct_info>&& si) {
    using sp_si = std::shared_ptr<struct_info>;
    sp_si* ud = static_cast<sp_si*>(lua_newuserdatatagged(L, sizeof(sp_si), tag));
    new (ud) sp_si(std::move(si));
    if (luaL_newmetatable(L, tname)) {
        lua_pushstring(L, tname);
        lua_setfield(L, -2, "__type");
        lua_setuserdatadtor(L, tag, [](lua_State* L, void* data) {
            static_cast<sp_si*>(data)->~sp_si();
        });
    }
    lua_setmetatable(L, -2);
}

std::shared_ptr<struct_info>& cinterop::check_struct_info(lua_State* L, int idx) {
    if (lua_userdatatag(L, idx) != tag) luaL_typeerrorL(L, idx, tname);
    return *static_cast<std::shared_ptr<struct_info>*>(lua_touserdatatagged(L, idx, tag));
}
bool cinterop::is_struct_info(lua_State* L, int idx) {
    return lua_userdatatag(L, idx) == tag;
}
std::shared_ptr<struct_info>& cinterop::to_struct_info(lua_State* L, int idx) {
    return *static_cast<std::shared_ptr<struct_info>*>(lua_touserdatatagged(L, idx, tag));
}
int cinterop::get_struct_field(lua_State* L) {
    auto& info = cinterop::check_struct_info(L, 1);
    size_t len;
    void* buf = luaL_checkbuffer(L, 2, &len);
    const char* key = luaL_checkstring(L, 3);
    if (len < info->memory_size) luaL_errorL(L, "buffer too small");
    if (not info->fields.contains(key)) luaL_errorL(L, "invalid field key");
    push_field(L, info->fields.at(key), buf);
    return 1;
}
int cinterop::set_struct_field(lua_State* L) {
    auto& info = cinterop::check_struct_info(L, 1);
    size_t len;
    void* buf = luaL_checkbuffer(L, 2, &len);
    const char* key = luaL_checkstring(L, 3);
    if (len < info->memory_size) luaL_errorL(L, "buffer too small");
    if (not info->fields.contains(key)) luaL_errorL(L, "invalid field key");
    set_field(L, info->fields[key], buf, 4);
    return 0;
}


int cinterop::create_struct_info(lua_State* L) {
    const int memory_size = luaL_checkinteger(L, 1);
    if (not lua_istable(L, 2)) luaL_typeerrorL(L, 2, "table");
    int table_index = lua_absindex(L, 2);
    bc::flat_map<std::string, struct_info::field_info> fields;
    lua_pushnil(L);
    while (lua_next(L, table_index) != 0) {
        std::string key = luaL_checkstring(L, -2);
        auto info = to_field_info(L, -1);
        fields.emplace(std::move(key), std::move(info));
        lua_pop(L, 1);
    }
    const int max_field_count = fields.size();
    std::shared_ptr<struct_info> si = std::make_shared<struct_info>(struct_info{
        .memory_size = memory_size,
        .fields = std::move(fields),
        .aggr{dcNewAggr(max_field_count, memory_size), dcFreeAggr},
    });
    register_fields(*si);
    dcCloseAggr(si->aggr.get());
    for (auto& [key, v] : si->fields) {
        std::cout << std::format("FIELD [{}]: {{type: {}, offset: {}, array_size: {}}}\n", key, int(v.type), v.memory_offset, v.array_size);
    }
    lua_newstructinfo(L, std::move(si));
    return 1;
};
