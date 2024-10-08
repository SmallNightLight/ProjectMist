#pragma once

#include "FPM/fixed.hpp"
#include "FPM/math.hpp"
#include "FPM/ios.hpp"
#include "FixedVector2.h"

#include <limits>

using Fixed8_8 = fpm::fixed<std::int16_t, std::int32_t, std::int8_t, std::int8_t, 8>;
using Fixed16_16 = fpm::fixed<std::int32_t, std::int64_t, int16_t, int16_t, 16, false>;

using Vector2 = FixedVector2<Fixed16_16, int16_t, int16_t>;

using Fixed16_16NoRounding = fpm::fixed<std::int32_t, std::int64_t, int16_t, int16_t, 16, false>;
using Fixed16_16Rounding = fpm::fixed<std::int32_t, std::int64_t, int16_t, int16_t, 16, true>;