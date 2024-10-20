#pragma once

#include "../../Math/FixedTypes.h"

struct RigidBody
{
    Vector2 Velocity;
    Vector2 RotationalVelocity;
    Fixed16_16 Density;
    Fixed16_16 Mass;
    Fixed16_16 Restitution;
    Fixed16_16 Area;

    RigidBody(
        const Vector2& velocity = Vector2(),
        const Vector2& rotationalVelocity = Vector2(),
        const Fixed16_16& density = Fixed16_16(1),
        const Fixed16_16& mass = Fixed16_16(1),
        const Fixed16_16& restitution = Fixed16_16(0, 5),
        const Fixed16_16& area = Fixed16_16(1))
        : Velocity(velocity),
          RotationalVelocity(rotationalVelocity),
          Density(density),
          Mass(mass),
          Restitution(restitution),
          Area(area) { }
};