#pragma once
#include <string>
#include <dluau.hpp>
using namespace dluau::type_aliases;

namespace cli {
struct Configuration {
    String sources;
    int optimization_level = 0;
    int debug_level = 0;
};
int run(const Configuration& config, const String* const args);
}
