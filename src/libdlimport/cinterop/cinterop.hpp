#pragma once
#include "../dlimport.hpp"
#include <dyncall_callback.h>
#include <any>
using namespace dlimport;

enum class c_type {
    c_aggregate = DC_SIGCHAR_AGGREGATE,
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
    c_char_ptr,
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
};
struct struct_object {
    std::shared_ptr<struct_info> info;
    std::any data;
};
class aggregate {
public:
    struct field {
        c_type type;
        int offset;
        int count;
    };
    aggregate(int max_fields, int size):
        max_fields_(max_fields), size_(size),
        ptr_(dcNewAggr(max_fields, size), dcFreeAggr) {
        fields_.reserve(max_fields);
    }
    void add_field(std::string name, field field);
    template <class T>
    T* to_field(void* data, const std::string& key) {
        return (T*)((char*)data + fields_.at(key).offset); 
    }
    template<class T>
    T& get_field(void* data, const std::string& fieldname) {
        return *(T*)((int8_t*)data + fields_.at(fieldname).offset);
    } 
    const boost::container::flat_map<std::string, field>& fields() const {return fields_;}
    DCaggr* get() const {return ptr_.get();}
    int size() const {return size_;}
private:
    boost::container::flat_map<std::string, field> fields_;
    std::unique_ptr<DCaggr, decltype(&dcFreeAggr)> ptr_;
    int size_;
    int max_fields_;
};
using aggregate_sp = std::shared_ptr<aggregate>;
inline const int aggregate_tag = dluau_newuserdatatag();
std::optional<c_type> string_to_param_type(std::string_view str);
namespace cinterop {
std::shared_ptr<struct_info>& check_struct_info(lua_State* L, int idx);
void push_c_types(lua_State* L);
int new_function_binding(lua_State* L);
int new_aggregate(lua_State* L);
int create_struct_info(lua_State* L);
}
