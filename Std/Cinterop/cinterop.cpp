#include "cinterop.hpp"
#include <print>
#include <dluau.h>

#define DLUAUSTD_CINTEROP_BUILD_TESTS


DLUAUSTD_API auto dlrequire(lua_State *L) -> int {
    const luaL_Reg lib[] = {
        {"struct_info", cinterop::create_struct_info},
        {"bind_fn", cinterop::new_function_binding},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    return 1;
}

#ifdef DLUAUSTD_CINTEROP_BUILD_TESTS

struct Test_struct {
    int some_int{};
    float some_float{};
    unsigned int some_unsigned_int{};
    char some_char{};
    unsigned char some_unsigned_char{};
    char some_char_arr[4];
};

DLUAUSTD_API void print_test_struct_info() {
    std::println(
        "Test_struct: sizeof({})"
        "\n(int) some_int: offsetof({}), sizeof({})"
        "\n(float) some_float: offsetof({}), sizeof({})"
        "\n(unsigned int) some_unsigned_int: offsetof({}), sizeof({})"
        "\n(char) some_char: offsetof({}), sizeof({})"
        "\n(char) some_unsigned_char: offsetof({}), sizeof({})"
        "\n(char[10]) some_char_arr: offsetof({}), sizeof({})",
        sizeof(Test_struct),
        offsetof(Test_struct, some_int), sizeof(Test_struct::some_int),
        offsetof(Test_struct, some_float), sizeof(Test_struct::some_float),
        offsetof(Test_struct, some_unsigned_int), sizeof(Test_struct::some_unsigned_int),
        offsetof(Test_struct, some_char), sizeof(Test_struct::some_char),
        offsetof(Test_struct, some_unsigned_char), sizeof(Test_struct::some_unsigned_char),
        offsetof(Test_struct, some_char_arr), sizeof(Test_struct::some_char_arr)
    );
} 
DLUAUSTD_API void print_struct_by_value(Test_struct obj) {
    auto* arr = obj.some_char_arr;
    std::println(
        "Test_struct: ("
        "\n\tsome_int: {}"
        "\n\tsome_float: {}"
        "\n\tsome_unsigned_int: {}"
        "\n\tsome_char: {}"
        "\n\tsome_unsigned_char: {}"
        "\n\tsome_char_arr: [{}, {}, {}, {}]"
        "\n)",
        obj.some_int, obj.some_float, obj.some_unsigned_int,
        obj.some_char, obj.some_unsigned_char,
        arr[0], arr[1], arr[2], arr[3]
    );
}
DLUAUSTD_API void print_struct_by_pointer(Test_struct* obj) {
    auto* arr = obj->some_char_arr;
    std::println(
        "Test_struct: ("
        "\n\tsome_int: {}"
        "\n\tsome_float: {}"
        "\n\tsome_unsigned_int: {}"
        "\n\tsome_char: {}"
        "\n\tsome_unsigned_char: {}"
        "\n\tsome_char_arr: [{}, {}, {}, {}]"
        "\n)",
        obj->some_int, obj->some_float, obj->some_unsigned_int,
        obj->some_char, obj->some_unsigned_char,
        arr[0], arr[1], arr[2], arr[3]
    );
}

DLUAUSTD_API auto test_function(long arg) -> int {
    std::println("This is a test function {}", arg);
    return 123;
}
DLUAUSTD_API auto dlinit(lua_State* L) -> void {
    std::println("CINTEROP WAS INITIALIZED {}", reinterpret_cast<uintptr_t>(L));
}
#endif
