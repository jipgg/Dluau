#include "cinterop.hpp"
constexpr const char* tname{"c_struct_info"};
static const int tag = dluau_registertypetagged(tname);
namespace bc = boost::container;

static struct_info::field_info to_field_info(lua_State* L, int idx) {
    lua_rawgetfield(L, idx, "type");
    if (not lua_isstring(L, -1)) luaL_errorL(L, "type must be a string");
    auto type_opt = string_to_param_type(lua_tostring(L, -1));
    if (not type_opt) luaL_errorL(L, "not a c_type");
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
    lua_newstructinfo(L, std::move(si));
    return 1;
};
