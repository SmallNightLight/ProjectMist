#pragma once

#include "FixedTypes.h"

struct AABB
{
    Vector2 Min;
    Vector2 Max;

    constexpr AABB() : Min(Vector2::Zero()), Max(Vector2::Zero()) { }
    constexpr AABB(const Vector2& min, const Vector2& max) : Min(min), Max(max) { }

    [[nodiscard]] constexpr bool Contains(const Vector2& point) const
    {
        return !(point.X < Min.X || point.Y < Min.Y || point.X >= (Max.X) || point.Y >= (Max.Y));
    }

    [[nodiscard]] constexpr bool Contains(const AABB& other) const
    {
        return (other.Min.X >= Min.X) && (other.Max.X < Max.X) &&
        (other.Min.Y >= Min.Y) && (other.Max.Y < Max.Y);
    }

    [[nodiscard]] constexpr  bool Overlaps(const AABB& other) const
    {
        return Min.X < other.Max.X && Max.X > other.Min.X &&
        Min.Y < other.Max.Y && Max.Y > other.Min.Y;
    }
};
