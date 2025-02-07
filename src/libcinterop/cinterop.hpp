#pragma once
#include "../libdlimport/dlimport.hpp"
#include <dyncall_callback.h>
#include <any>
using namespace dlimport;

enum class c_type {
    c_int = DC_SIGCHAR_INT,
    c_uint = DC_SIGCHAR_UINT,
    c_ulong = DC_SIGCHAR_ULONG,
    c_long = DC_SIGCHAR_LONG,
    c_ushort = DC_SIGCHAR_USHORT,
    c_char = DC_SIGCHAR_CHAR,
    c_uchar = DC_SIGCHAR_UCHAR,
    c_short = DC_SIGCHAR_SHORT,
    c_void = DC_SIGCHAR_VOID,
    c_float = DC_SIGCHAR_FLOAT,
    c_double = DC_SIGCHAR_DOUBLE,
    c_void_ptr = DC_SIGCHAR_POINTER,
    c_string,
    c_bool = DC_SIGCHAR_BOOL,
};
struct struct_info {
    int memory_size;
    struct field_info {
        c_type type;
        int memory_offset;
        int array_size;
    };
    boost::container::flat_map<std::string, field_info> fields;
    std::unique_ptr<DCaggr, decltype(&dcFreeAggr)> aggr;
    std::unique_ptr<int, std::function<void(int*)>> metatable;
    void* newinstance(lua_State* L);
};
std::optional<c_type> string_to_param_type(std::string_view str);
namespace cinterop {
std::shared_ptr<struct_info>& check_struct_info(lua_State* L, int idx);
std::shared_ptr<struct_info>& to_struct_info(lua_State* L, int idx);
int new_struct_instance(lua_State* L);
bool is_struct_info(lua_State* L, int idx);
void push_c_types(lua_State* L);
int new_function_binding(lua_State* L);
int c_type_sizeof(lua_State* L);
int create_struct_info(lua_State* L);
int get_struct_field(lua_State* L);
int set_struct_field(lua_State* L);
}
