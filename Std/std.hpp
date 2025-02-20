#pragma once
#ifdef DLUAUSTD_EXPORT
#define DLUAUSTD_API extern "C" __declspec(dllexport)
#else
#define DLUAUSTD_API extern "C" __declspec(dllimport)
#endif
#include <memory>
#include <dluau.hpp>
#include "Lazy_type.hpp"
#include <iostream>

namespace dluaustd {
using Reader = std::unique_ptr<std::istream>;
using Writer = std::unique_ptr<std::ostream>;
struct Reader_type_info {
    static consteval const char* type_namespace() {return "std";}
    static consteval const char* type_name() {return "reader";}
};
using Reader_type = Lazy_type<Reader, Reader_type_info>;
struct Writer_type_info {
    static consteval const char* type_namespace() {return "std";}
    static consteval const char* type_name() {return "writer";}
};
using Writer_type = Lazy_type<Writer, Writer_type_info>;
}
