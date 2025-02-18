#pragma once
#include <type_traits>

template <class T>
concept Numeric = std::is_integral_v<T> or std::is_floating_point_v<T>;
