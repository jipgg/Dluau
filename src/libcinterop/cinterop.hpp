#pragma once
#include "../libdlimport/dlimport.hpp"
using namespace dlimport;

enum class C_type {
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
struct Struct_info {
    int memory_size;
    struct Field_info {
        C_type type;
        int memory_offset;
        int array_size;
    };
    Flat_map<String, Field_info> fields;
    Unique<DCaggr, decltype(&dcFreeAggr)> aggr;
    Unique<int, Func<void(int*)>> metatable;
    void* newinstance(Lstate L);
};
Opt<C_type> string_to_param_type(Str_view str);
namespace cinterop {
Shared<Struct_info>& check_struct_info(Lstate L, int idx);
Shared<Struct_info>& to_struct_info(Lstate L, int idx);
int new_struct_instance(Lstate L);
bool is_struct_info(Lstate L, int idx);
void push_c_types(Lstate L);
int new_function_binding(Lstate L);
int c_type_sizeof(Lstate L);
int create_struct_info(Lstate L);
int get_struct_field(Lstate L);
int set_struct_field(Lstate L);
}
