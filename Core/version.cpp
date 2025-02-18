#include <dluau.hpp>
#include <iostream>
#include <string>
#include <format>
using std::string;

DLUAU_API auto get_version() -> const char* {
    static const string version = std::format(
        "{}.{}.{}", DLUAUCORE_VERSION_MAJOR,
        DLUAUCORE_VERSION_MINOR, DLUAUCORE_VERSION_PATCH
    );
    return version.c_str();
}
DLUAU_API auto get_version_major() -> int {
    return DLUAUCORE_VERSION_MAJOR;
}
DLUAU_API auto get_version_minor() -> int {
    return DLUAUCORE_VERSION_MINOR;
}
DLUAU_API auto get_version_patch() -> int {
    return DLUAUCORE_VERSION_PATCH;
}
#ifndef NDEBUG
DLUAU_API auto test_unsigned_int_return() -> unsigned int {
    return 0xfeu;
}
DLUAU_API auto test_int_return() -> int {
    return 0xfe;
}
DLUAU_API auto test_unsigned_int_arg(unsigned int e) -> unsigned int {
    return e;
}
DLUAU_API auto test_int_arg(int e) -> int {
    return e;
}
struct Point {
    int x;
    int y;
};
struct Test {
    int a;
    char abc[3];
    bool yes;
    float flt;
    double num;
    unsigned char uch;
};
DLUAU_API void print_test_info() {
    std::cout << std::format("[size] {}\n", sizeof(Test));
    std::cout << std::format("[field] a: {}\n", offsetof(Test, a));
    std::cout << std::format("[field] abc: {}\n", offsetof(Test, abc));
    std::cout << std::format("[field] yes: {}\n", offsetof(Test, yes));
    std::cout << std::format("[field] flt: {}\n", offsetof(Test, flt));
    std::cout << std::format("[field] num: {}\n", offsetof(Test, num));
    std::cout << std::format("[field] uch: {}\n", offsetof(Test, uch));
}
DLUAU_API auto test_point(int x, int y) -> Point {
    std::cout << std::format("X: {}, Y: {}\n", x, y);
    std::cout << std::format("SIZE: {}, XOFF: {}, YOFF {}\n", sizeof(Point), offsetof(Point, x), offsetof(Point, y));
    return Point{.x = x, .y = y};
}
DLUAU_API void print_point(Point point) {
    std::cout << std::format("POINT: {{X: {}, Y: {}}}\n", point.x, point.y);
}
DLUAU_API auto test_unsigned_char_return() -> unsigned char {
    return 0xfeui8;
}
#endif
