#include "dluau.h"
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
DLUAU_API unsigned char test_unsigned_char_return() {
    return 0xfeui8;
}
