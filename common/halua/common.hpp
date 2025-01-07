#pragma once
#include <string_view>
#include <lualib.h>

namespace halua {
int type_hash(std::string_view name, uint32_t seed = 0);

constexpr uint16_t simple_hash(const std::string_view str) {
    uint16_t hash = 0;
    for (char c : str) {
        hash = (hash * 31) ^ static_cast<uint16_t>(c);
    }
    return hash;
}
}
