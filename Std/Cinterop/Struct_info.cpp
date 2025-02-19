#include "cinterop.hpp"
#include <bit>
constexpr const char* tname{"structinfo"};
static const int tag = dluau_registertypetagged(tname);
using Field_info = Struct_info::Field_info;
using Registry = Struct_info_type::Registry;
using Init_info = Struct_info_type::Init_info;
using std::bit_cast;

using namespace cinterop;

static auto to_field_info(lua_State* L, int idx) -> Field_info {
    lua_rawgetfield(L, idx, "type");
    if (not lua_isstring(L, -1)) luaL_errorL(L, "type must be a string");
    auto type_opt = string_to_param_type(lua_tostring(L, -1));
    if (not type_opt) luaL_errorL(L, "not a c_type '%s'", lua_tostring(L, -1));
    lua_pop(L, 1);
    lua_rawgetfield(L, idx, "memory_offset");
    if (not lua_isnumber(L, -1)) luaL_errorL(L, "memory_offset must be a number");
    const int offset = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lua_rawgetfield(L, idx, "array_size");
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
auto Struct_info::newinstance(lua_State* L) -> void* {
    void* ud = lua_newuserdata(L, memory_size);
    memset(ud, 0, memory_size);
    if (metatable != nullptr) {
        lua_getref(L, *metatable);
        lua_setmetatable(L, -2);
    }
    return ud;
}

static void set_field(lua_State* L, const Field_info& fi, void* data, int idx, int n = 0) {
    void* off = static_cast<int8_t*>(data) + fi.memory_offset;
    if (n >= fi.array_size) luaL_errorL(L, "out of range");
    switch(fi.type) {
        case Native_type::Bool:
            *(static_cast<bool*>(off) + n) = luaL_checkboolean(L, idx);
            break;
        case Native_type::Char:
            *(static_cast<char*>(off) + n) = luaL_checknumber(L, idx);
            break;
        case Native_type::Uchar:
            *(static_cast<unsigned char*>(off) + n) = luaL_checknumber(L, idx);
            break;
        case Native_type::Short:
            *(static_cast<short*>(off) + n) = luaL_checknumber(L, idx);
            break;
        case Native_type::Ushort:
            *(static_cast<unsigned short*>(off) + n) = luaL_checknumber(L, idx);
            break;
        case Native_type::Long:
            *(static_cast<long*>(off) + n) = luaL_checknumber(L, idx);
            break;
        case Native_type::Ulong:
            *(static_cast<unsigned long*>(off) + n) = luaL_checknumber(L, idx);
            break;
        case Native_type::Int:
            *(static_cast<int*>(off) + n) = luaL_checknumber(L, idx);
            break;
        case Native_type::Uint:
            *(static_cast<unsigned int*>(off) + n) = luaL_checknumber(L, idx);
            break;
        case Native_type::Float:
            *(static_cast<float*>(off) + n) = luaL_checknumber(L, idx);
            break;
        case Native_type::Double:
            *(static_cast<double*>(off) + n) = luaL_checknumber(L, idx);
            break;
        case Native_type::Void_ptr:
            *(static_cast<void**>(off) + n) = lua_tolightuserdata(L, idx);
            break;
        case Native_type::Void:
            luaL_errorL(L, "cannot set void");
            break;
        case Native_type::Char_ptr:
            luaL_errorL(L, "cannot set string");
            break;
    }
}

static void push_field(lua_State* L, const Field_info& fi, void* data, int n = 0) {
    void* off = static_cast<int8_t*>(data) + fi.memory_offset;
    if (n >= fi.array_size) luaL_errorL(L, "out of range");
    switch(fi.type) {
        case Native_type::Bool:
            lua_pushboolean(L, *(static_cast<bool*>(off) + n));
            break;
        case Native_type::Char:
            lua_pushnumber(L, *(static_cast<char*>(off) + n));
            break;
        case Native_type::Uchar:
            lua_pushnumber(L, *(static_cast<unsigned char*>(off) + n));
            break;
        case Native_type::Short:
            lua_pushnumber(L, *(static_cast<short*>(off) + n));
            break;
        case Native_type::Ushort:
            lua_pushnumber(L, *(static_cast<unsigned short*>(off) + n));
            break;
        case Native_type::Long:
            lua_pushnumber(L, *(static_cast<long*>(off) + n));
            break;
        case Native_type::Ulong:
            lua_pushnumber(L, *(static_cast<unsigned long*>(off) + n));
            break;
        case Native_type::Int:
            lua_pushnumber(L, *(static_cast<int*>(off) + n));
            break;
        case Native_type::Uint:
            lua_pushnumber(L, *(static_cast<unsigned int*>(off) + n));
            break;
        case Native_type::Float:
            lua_pushnumber(L, *(static_cast<float*>(off) + n));
            break;
        case Native_type::Double:
            lua_pushnumber(L, *(static_cast<double*>(off) + n));
            break;
        case Native_type::Void_ptr:
            lua_pushlightuserdata(L, *(static_cast<void**>(off) + n));
            break;
        case Native_type::Void:
            lua_pushnil(L);
            break;
        case Native_type::Char_ptr:
            lua_pushstring(L, *(static_cast<const char**>(off) + n));
            break;
    }
}
static auto get_struct_field(lua_State* L) -> int {
    auto& info = Struct_info_type::check(L, 1);
    void* data = lua_touserdata(L, 2);
    const char* key = luaL_checkstring(L, 3);
    const int arrindex = luaL_optinteger(L, 4, 0);
    if (not info->fields.contains(key)) luaL_errorL(L, "invalid field key");
    push_field(L, info->fields.at(key), data, arrindex);
    return 1;
}
static auto set_struct_field(lua_State* L) -> int {
    auto& info = Struct_info_type::check(L, 1);
    void* data = lua_touserdata(L, 2);
    const char* key = luaL_checkstring(L, 3);
    const int arrindex = luaL_optinteger(L, 5, 0);
    if (not info->fields.contains(key)) luaL_errorL(L, "invalid field key");
    set_field(L, info->fields[key], data, 4, arrindex);
    return 0;
}
static auto new_struct_instance(lua_State* L) -> int {
    auto& info = Struct_info_type::check(L, 1);
    info->newinstance(L);
    return 1;
}

auto cinterop::create_struct_info(lua_State* L) -> int {
    const int memory_size = luaL_checkinteger(L, 1);
    if (not lua_istable(L, 2)) luaL_typeerrorL(L, 2, "table");
    int table_index = lua_absindex(L, 2);
    boost::container::flat_map<std::string, Field_info> fields;
    lua_pushnil(L);
    while (lua_next(L, table_index) != 0) {
        std::string key = luaL_checkstring(L, -2);
        auto info = to_field_info(L, -1);
        fields.emplace(std::move(key), std::move(info));
        lua_pop(L, 1);
    }
    const int max_field_count = fields.size();
    std::unique_ptr<int, std::function<void(int*)>> metatable{nullptr, [M = lua_mainthread(L)](int* ref) {
        lua_unref(M, *ref);
    }};
    if (lua_istable(L, 3)) {
        metatable.reset(new int(lua_ref(L, 3)));
    }
    std::shared_ptr<Struct_info> si = std::make_shared<Struct_info>(Struct_info{
        .memory_size = memory_size,
        .fields = std::move(fields),
        .aggr{dcNewAggr(max_field_count, memory_size), dcFreeAggr},
        .metatable = std::move(metatable),
    });
    register_fields(*si);
    dcCloseAggr(si->aggr.get());
    Struct_info_type::make(L, std::move(si));
    return 1;
};

static const Registry namecalls{
    {"get_field", [](lua_State* L, auto& s) -> int {
        void* data = lua_touserdata(L, 2);
        const char* key = luaL_checkstring(L, 3);
        const int arrindex = luaL_optinteger(L, 4, 0);
        if (not s->fields.contains(key)) luaL_errorL(L, "invalid field key");
        push_field(L, s->fields.at(key), data, arrindex);
        return 1;
    }},
    {"set_field", [](lua_State* L, auto& s) {
        void* data = lua_touserdata(L, 2);
        const char* key = luaL_checkstring(L, 3);
        const int arrindex = luaL_optinteger(L, 5, 0);
        if (not s->fields.contains(key)) luaL_errorL(L, "invalid field key");
        set_field(L, s->fields[key], data, 4, arrindex);
        return 0;
    }},
    {"new_instance", [](lua_State* L, auto& s) {
        s->newinstance(L);
        return 1;
    }},
    {"to_pointer", [](lua_State* L, std::shared_ptr<Struct_info>& s) -> int {
        lua_pushlightuserdata(L, (void*)lua_topointer(L, 2));
        return 1;
    }}
};
template<> const Init_info Struct_info_type::init_info{
    .namecall = namecalls,
};
