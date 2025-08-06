#pragma once

#include "../Math/FixedTypes.h"

struct Transform
{
    Vector2 Position;

    inline Transform() noexcept = default;
    constexpr inline explicit Transform(Fixed16_16 x, Fixed16_16 y) : Position(x, y) { }
    constexpr inline explicit Transform(const Vector2& value) : Position(value) { }
};