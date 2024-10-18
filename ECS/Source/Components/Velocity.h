#pragma once

#include "../Math/FixedTypes.h"

struct Velocity
{
    Vector2 Value;
    Vector2 Acceleration;

    Velocity() : Value(0, 0), Acceleration(Vector2{0, 0}) { }
    Velocity(Fixed16_16 x, Fixed16_16 y) : Value(x, y), Acceleration(Vector2{0, 0}) { }
    explicit Velocity(const Vector2& value) : Value(value), Acceleration(Vector2{0, 0}) { }
};