#pragma once

#include "../../Math/FixedTypes.h"

struct Rigidbody
{
    Vector2 Velocity;
    Vector2 RotationalVelocity;
    Fixed16_16 Density;
    Fixed16_16 Mass;
    Fixed16_16 Restitution;
    Fixed16_16 Area;
};