#include "cinterop.hpp"
#include <boost/container/flat_map.hpp>
#include <memory>
#include <any>
using std::string, std::optional;
using std::string_view;
using boost::container::flat_map;
static boost::container::flat_map<c_type, DCsigchar> param_map{
    {c_type::c_double, DC_SIGCHAR_DOUBLE},
    {c_type::c_bool, DC_SIGCHAR_BOOL},
    {c_type::c_void_ptr, DC_SIGCHAR_POINTER},
    {c_type::c_float, DC_SIGCHAR_FLOAT},
    {c_type::c_int, DC_SIGCHAR_INT},
    {c_type::c_void, DC_SIGCHAR_VOID},
    {c_type::c_long, DC_SIGCHAR_LONG},
    {c_type::c_ulong, DC_SIGCHAR_ULONG},
    {c_type::c_short, DC_SIGCHAR_SHORT},
    {c_type::c_ushort, DC_SIGCHAR_USHORT},
    {c_type::c_char, DC_SIGCHAR_CHAR},
    {c_type::c_uchar, DC_SIGCHAR_UCHAR},
};

void aggregate::add_field(string name, field field) {
    dcAggrField(ptr_.get(), param_map[field.type], field.offset, field.count);
}

int cinterop::new_aggregate(lua_State *L) {
    aggregate aggr{lua_objlen(L, 2), luaL_checkinteger(L, 1)};
    for (int i{1}; i <= lua_objlen(L, 2); ++i) {
        lua_rawgeti(L, 4, i);
        lua_rawgetfield(L, -1, "name");
        const string name = luaL_checkstring(L, -1);
        lua_pop(L, 1);
        lua_rawgetfield(L, -1, "type");
        const optional<c_type> type = string_to_param_type(luaL_checkstring(L, -1));
        if (not type) luaL_errorL(L, "unknown param type '%s'", luaL_checkstring(L, -1));
        lua_pop(L, 1);
        lua_rawgetfield(L, -1, "memory_offset");
        const int offset = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_rawgetfield(L, -1, "array_size");
        const int count = luaL_optinteger(L, -1, 0);
        lua_pop(L, 2);
        aggr.add_field(name, {
            .type = *type,
            .offset = offset,
            .count = count
        });
    }
    aggregate_sp& sp = *static_cast<aggregate_sp*>(lua_newuserdatatagged(L, sizeof(aggregate_sp), aggregate_tag));
    sp = std::make_shared<aggregate>(std::move(aggr));
    if (luaL_newmetatable(L, "c_aggregate")) {
        lua_pushstring(L, "c_aggregate");
        lua_setfield(L, -2, "__type");
        lua_setuserdatadtor(L, aggregate_tag, [](lua_State* L, void* ud) {
            static_cast<aggregate_sp*>(ud)->~aggregate_sp();
        });
    }
    lua_setmetatable(L, -2);
    return 1;
}
