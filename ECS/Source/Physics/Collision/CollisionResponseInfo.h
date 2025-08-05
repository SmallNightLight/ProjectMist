#pragma once

#include "../../Math/FixedTypes.h"

struct CollisionResponseInfo
{
    inline constexpr CollisionResponseInfo() :
        Position1(Vector2(0, 0)),
        Position2(Vector2(0, 0)),
        Velocity1(Vector2(0, 0)),
        Velocity2(Vector2(0, 0)),
        AngularVelocity1(Fixed16_16(0)),
        AngularVelocity2(Fixed16_16(0)) { }

    inline constexpr CollisionResponseInfo(Vector2 position1, Vector2 position2, Vector2 velocity1, Vector2 velocity2, Fixed16_16 angularVelocity1, Fixed16_16 angularVelocity2) :
        Position1(position1),
        Position2(position2),
        Velocity1(velocity1),
        Velocity2(velocity2),
        AngularVelocity1(angularVelocity1),
        AngularVelocity2(angularVelocity2) { }

    Vector2 Position1;
    Vector2 Position2;
    Vector2 Velocity1;
    Vector2 Velocity2;
    Fixed16_16 AngularVelocity1;
    Fixed16_16 AngularVelocity2;
};