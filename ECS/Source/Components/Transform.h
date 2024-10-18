#pragma once

#include "../Math/FixedTypes.h"

struct Transform
{
    Vector2 Position;

    Transform() : Position(0, 0) { }
    Transform(Fixed16_16 x, Fixed16_16 y) : Position(x, y) { }
    explicit Transform(const Vector2& value) : Position(value) { }
};