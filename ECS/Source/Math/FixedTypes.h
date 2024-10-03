#pragma once

#include "FPM/fixed.hpp"
#include "FPM/math.hpp"
#include "FPM/ios.hpp"
#include "FixedVector2.h"

#include <limits>

using Fixed8_8 = fpm::fixed<std::int16_t, std::int32_t, 8>;
using Fixed16_16 = fpm::fixed<std::int32_t, std::int64_t, 16>;

using Vector2 = FixedVector2<Fixed16_16>;

constexpr std::int32_t IntPow(std::int32_t x, std::int32_t p)
{
    std::int32_t result = 1;
    while (p > 0)
    {
        if (p % 2 == 1)
        {
            result *= x;
        }

        x *= x;
        p /= 2;
    }
    return result;
}

constexpr std::int32_t GetDigits(int32_t num)
{
    if (num == 0) return 1;
    std::int32_t digits = 0;
    while (num != 0)
    {
        num /= 10;
        digits++;
    }
    return digits;
}

constexpr int MultiplyFixed(const int32_t value1, const int32_t integerPart, const int32_t fractionalPart)
{
    const int32_t scale = IntPow(10, GetDigits(fractionalPart));
    const int32_t combined_value = integerPart * scale + fractionalPart;
    return value1 * combined_value / scale;
}

constexpr Fixed16_16 GetFixed16_16(const int16_t integerPart, const int16_t fractionalPart)
{
    const int32_t digits = GetDigits(fractionalPart);
    const int32_t bound = 1 << 16;
    const int32_t divisor = IntPow(10, digits);
    const int32_t decimal = MultiplyFixed(fractionalPart, bound / divisor, bound % divisor);

    return Fixed16_16::from_raw_value(integerPart < 0 ? integerPart * bound - decimal : integerPart * bound + decimal);
}

constexpr Fixed16_16 GetFixed16_16(const int integerPart)
{
    return Fixed16_16::from_raw_value(integerPart << 16);
}