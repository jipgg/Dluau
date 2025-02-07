#include "dluau.h"
#include <iostream>
#include <string>
using std::string;
#include <format>

DLUAU_API const char* get_version() {
    static const string version = std::format("{}.{}.{}", DLUAU_VERSION_MAJOR, DLUAU_VERSION_MINOR, DLUAU_VERSION_PATCH);
    return version.c_str();
}
DLUAU_API int get_version_major() {
    return DLUAU_VERSION_MAJOR;
}
DLUAU_API int get_version_minor() {
    return DLUAU_VERSION_MINOR;
}
DLUAU_API int get_version_patch() {
    return DLUAU_VERSION_PATCH;
}
DLUAU_API unsigned int test_unsigned_int_return() {
    return 0xfeu;
}
DLUAU_API int test_int_return() {
    return 0xfe;
}
DLUAU_API unsigned int test_unsigned_int_arg(unsigned int e) {
    return e;
}
DLUAU_API int test_int_arg(int e) {
    return e;
}
struct point {
    int x;
    int y;
};
DLUAU_API point test_point(int x, int y) {
    std::cout << std::format("X: {}, Y: {}\n", x, y);
    std::cout << std::format("SIZE: {}, XOFF: {}, YOFF {}\n", sizeof(point), offsetof(point, x), offsetof(point, y));
    return point{.x = x, .y = y};
}
DLUAU_API void print_point(point point) {
    std::cout << std::format("POINT: {{X: {}, Y: {}}}\n", point.x, point.y);
}
DLUAU_API unsigned char test_unsigned_char_return() {
    return 0xfeui8;
}
