#pragma once
#include <string>
#include <dluau.hpp>

namespace cli {
struct Configuration {
    std::string sources;
    int optimization_level = 0;
    int debug_level = 0;
};
auto run(const Configuration& config, const std::string* const args) -> int;
}
