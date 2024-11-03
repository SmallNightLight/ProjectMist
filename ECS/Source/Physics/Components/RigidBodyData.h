#pragma once

#include "../../Math/FixedTypes.h"

#include <cassert>

struct RigidBodyData
{
    Vector2 Velocity;
    Fixed16_16 RotationalVelocity;
    Vector2 Force;

    Fixed16_16 Density;
    Fixed16_16 InverseMass;
    Fixed16_16 Restitution;
    Fixed16_16 Area;

    RigidBodyData() : Velocity(0, 0), RotationalVelocity(0, 0), Density(1), InverseMass(1), Restitution(0, 5), Area(0), Force(0, 0) { }

    constexpr RigidBodyData(const Fixed16_16& density, const Fixed16_16& mass, const Fixed16_16& restitution, const Fixed16_16& area)
        : Velocity(0, 0),
          RotationalVelocity(0, 0),
          Density(density),
          InverseMass(1 / mass),
          Restitution(restitution),
          Area(area),
          Force(0, 0)
    { }

    constexpr static RigidBodyData CreateBoxRigidBody(const Fixed16_16& density, const Fixed16_16& restitution, const Fixed16_16 width, const Fixed16_16 height)
    {
        Fixed16_16 area = width * height;
        assert(area > Fixed16_16(0) && density > Fixed16_16(0) && restitution >= Fixed16_16(0) && restitution <= Fixed16_16(1) && "Invalid properties of rigidBody");
        return RigidBodyData{density, area * density, restitution, area };
    }

    constexpr static RigidBodyData CreateCircleRigidBody(const Fixed16_16& density, const Fixed16_16& restitution, const Fixed16_16 radius)
    {
        Fixed16_16 area = radius * radius * Fixed16_16::pi();
        assert(area > Fixed16_16(0) && density > Fixed16_16(0) && restitution >= Fixed16_16(0) && restitution <= Fixed16_16(1) && "Invalid properties of rigidBody");
        return RigidBodyData{density, area * density, restitution, area };
    }

    void ApplyForce(const Vector2& direction)
    {
        Force += direction;
    }

    [[nodiscard]] Fixed16_16 GetMass() const
    {
        return 1 / InverseMass;
    }
};