#pragma once

#include "../Math/FixedTypes.h"

struct Boid
{
    Vector2 Velocity;
    Vector2 Acceleration;

    Boid(): Velocity(0, 0), Acceleration(0, 0) { }

    Boid(const Vector2& velocity, const Vector2& acceleration) : Velocity(velocity), Acceleration(acceleration) { }

};