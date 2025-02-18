#pragma once
#include "gpm_api.h"
#include <memory>
#include "LazyUserdataType.hpp"
#include <iostream>

namespace gpm {
using Reader = std::unique_ptr<std::istream>;
using Writer = std::unique_ptr<std::ostream>;
struct ReaderTypeInfo {
    static consteval const char* type_namespace() {return "gpm";}
    static consteval const char* type_name() {return "Reader";}
};
using ReaderType = LazyUserdataType<Reader, ReaderTypeInfo>;
struct WriterTypeInfo {
    static consteval const char* type_namespace() {return "gpm";}
    static consteval const char* type_name() {return "Writer";}
};
using WriterType = LazyUserdataType<Writer, WriterTypeInfo>;
}
