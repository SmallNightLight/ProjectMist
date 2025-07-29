#pragma once

#include <functional>

constexpr inline uint32_t CombineHash(uint32_t a, uint32_t b)
{
    return a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2));
}