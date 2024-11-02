#pragma once

#include "FixedTypes.h"

struct AABB
{
    Vector2 Min;
    Vector2 Max;

    constexpr AABB() : Min(Vector2::Zero()), Max(Vector2::Zero()) { }
    constexpr AABB(const Vector2& min, const Vector2& max) : Min(min), Max(max) { }
};
