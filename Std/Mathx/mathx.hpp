#pragma once
#include <dluau.h>
#include <blaze/Blaze.h>
#include "misc.hpp"
#include "VectorType.hpp"
#include "SquareMatrixType.hpp"
#include <array>

using V3Type = VectorType<double, 3>;

using V3fType = VectorType<float, 3>;
using V3i16Type = VectorType<int16_t, 3>;

using V4Type = VectorType<double, 4>;
using V4fType = VectorType<float, 4>;
using V4i16Type = VectorType<int16_t, 4>;

using V2Type = VectorType<double, 2>;
using V2fType = VectorType<float, 2>;
using V2i16Type = VectorType<int16_t, 2>;

using M3Type = SquareMatrixType<double, 3>;
using M3fType = SquareMatrixType<float, 3>;
using M3i16Type = SquareMatrixType<float, 3>;

using M4Type = SquareMatrixType<double, 4>;
using M4fType = SquareMatrixType<float, 4>;

using M4i16Type = SquareMatrixType<float, 4>;
