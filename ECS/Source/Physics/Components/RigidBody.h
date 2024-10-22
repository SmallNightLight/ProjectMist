#pragma once

#include "../../Math/FixedTypes.h"

#include <cassert>

struct RigidBody
{
    Vector2 Velocity;
    Vector2 RotationalVelocity;
    Fixed16_16 Density;
    Fixed16_16 Mass;
    Fixed16_16 Restitution;
    Fixed16_16 Area;

    RigidBody() : Velocity(0, 0), RotationalVelocity(0, 0), Density(0), Mass(0), Restitution(0), Area(0) { }

    constexpr RigidBody(const Fixed16_16& density, const Fixed16_16& mass, const Fixed16_16& restitution, const Fixed16_16& area)
        : Velocity(0, 0),
          RotationalVelocity(0, 0),
          Density(density),
          Mass(mass),
          Restitution(restitution),
          Area(area)
    { }

    constexpr static RigidBody CreateBoxRigidBody(const Fixed16_16& density, const Fixed16_16& restitution, const Fixed16_16 width, const Fixed16_16 height)
    {
        Fixed16_16 area = width * height;
        assert(area > Fixed16_16(0) && density > Fixed16_16(0) && restitution >= Fixed16_16(0) && restitution <= Fixed16_16(1) && "Invalid properties of rigidBody");
        return RigidBody{density, area * density, restitution, area};
    }

    constexpr static RigidBody CreateCircleRigidBody(const Fixed16_16& density, const Fixed16_16& restitution, const Fixed16_16 radius)
    {
        Fixed16_16 area = radius * radius * Fixed16_16::pi();
        assert(area > Fixed16_16(0) && density > Fixed16_16(0) && restitution >= Fixed16_16(0) && restitution <= Fixed16_16(1) && "Invalid properties of rigidBody");
        return RigidBody{density, area * density, restitution, area};
    }
};