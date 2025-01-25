#pragma once
#include <string>

namespace cli {
struct configuration {
    std::string sources;
    int optimization_level = 0;
    int debug_level = 0;
};
int run(const configuration& config, const std::string* const args);
}
