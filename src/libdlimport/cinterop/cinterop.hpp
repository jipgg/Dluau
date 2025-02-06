#pragma once
#include "../dlimport.hpp"
using namespace dlimport;

enum class c_type {
    c_int,
    c_uint,
    c_ulong,
    c_long,
    c_ushort,
    c_char,
    c_uchar,
    c_short,
    c_void,
    c_float,
    c_double,
    c_void_ptr,
    c_char_ptr,
    c_bool,
};
std::optional<c_type> string_to_param_type(std::string_view str);
namespace cinterop {
void push_c_types(lua_State* L);
int new_function_binding(lua_State* L);
}
