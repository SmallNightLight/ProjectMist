#pragma once

#include "../Math/FixedTypes.h"

struct Boid
{
    Vector2 Velocity;
    Vector2 Acceleration;

    inline Boid() noexcept = default;

    constexpr inline explicit Boid(const Vector2& velocity, const Vector2& acceleration) : Velocity(velocity), Acceleration(acceleration) { }
};