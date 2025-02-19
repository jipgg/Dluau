#pragma once
#include <dyncall.h>
#include <string>
#include <dluau.h>
#include <optional>
#include <functional>
#include <memory>
#include <boost/container/flat_map.hpp>
#include <LazyUserdataType.hpp>

constexpr const char* cinterop_namespace{"gpm.cinterop"};
struct CinteropNamespace {
    static consteval const char* type_namespace() {return "gpm.cinterop";}
};
enum class NativeType {
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
    VoidPtr = DC_SIGCHAR_POINTER,
    String,
    Bool = DC_SIGCHAR_BOOL,
};
struct StructInfo {
    int memory_size;
    struct FieldInfo {
        NativeType type;
        int memory_offset;
        int array_size;
    };
    boost::container::flat_map<std::string, FieldInfo> fields;
    std::unique_ptr<DCaggr, decltype(&dcFreeAggr)> aggr;
    std::unique_ptr<int, std::function<void(int*)>> metatable;
    void* newinstance(lua_State* L);
};
struct StructInfoTypeInfo: public CinteropNamespace {
    static consteval const char* type_name() {return "StructInfo";}
};
using StructInfoType = LazyUserdataType<std::shared_ptr<StructInfo>, StructInfoTypeInfo>;
auto string_to_param_type(std::string_view str) -> std::optional<NativeType>;
namespace cinterop {
auto new_struct_instance(lua_State* L) -> int;
auto new_function_binding(lua_State* L) -> int;
auto create_struct_info(lua_State* L) -> int;
}

