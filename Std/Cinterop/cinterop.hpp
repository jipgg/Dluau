#pragma once
#include <dyncall.h>
#include <string>
#include <dluau.h>
#include <optional>
#include <functional>
#include <memory>
#include <boost/container/flat_map.hpp>
#include <std.hpp>

struct Cinterop_namespace {
    static consteval const char* type_namespace() {return "std.cinterop";}
};
enum class Native_type {
    Int = DC_SIGCHAR_INT,
    Uint = DC_SIGCHAR_UINT,
    Ulong = DC_SIGCHAR_ULONG,
    Long = DC_SIGCHAR_LONG,
    Ushort = DC_SIGCHAR_USHORT,
    Char = DC_SIGCHAR_CHAR,
    Uchar = DC_SIGCHAR_UCHAR,
    Short = DC_SIGCHAR_SHORT,
    Void = DC_SIGCHAR_VOID,
    Float = DC_SIGCHAR_FLOAT,
    Double = DC_SIGCHAR_DOUBLE,
    Void_ptr = DC_SIGCHAR_POINTER,
    Char_ptr,
    Bool = DC_SIGCHAR_BOOL,
};
struct Struct_info {
    int memory_size;
    struct Field_info {
        Native_type type;
        int memory_offset;
        int array_size;
    };
    boost::container::flat_map<std::string, Field_info> fields;
    std::unique_ptr<DCaggr, decltype(&dcFreeAggr)> aggr;
    std::unique_ptr<int, std::function<void(int*)>> metatable;
    void* newinstance(lua_State* L);
};
struct Struct_info_type_info: public Cinterop_namespace {
    static consteval const char* type_name() {return "struct_info";}
};
using Struct_info_type = Lazy_type<std::shared_ptr<Struct_info>, Struct_info_type_info>;
auto string_to_param_type(std::string_view str) -> std::optional<Native_type>;
namespace cinterop {
auto new_struct_instance(lua_State* L) -> int;
auto new_function_binding(lua_State* L) -> int;
auto create_struct_info(lua_State* L) -> int;
}

