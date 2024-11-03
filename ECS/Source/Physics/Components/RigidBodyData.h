#pragma once

#include "../../Math/FixedTypes.h"

#include <cassert>

struct RigidBodyData
{
    Vector2 Velocity;
    Fixed16_16 AngularVelocity;
    Vector2 Force;

    Fixed16_16 InverseMass;
    Fixed16_16 Restitution;
    Fixed16_16 Area; //TODO: Remove area?
    Fixed16_16 InverseInertia;
    Fixed16_16 StaticFriction; //TODO: add materials and only save the index instead of he entire friction value (Restitution, StaticFriction, DynamicFriction)
    Fixed16_16 DynamicFriction;

    RigidBodyData() : Velocity(0, 0), AngularVelocity(0, 0), Force(0, 0), InverseMass(1), Restitution(0, 5), Area(0), InverseInertia(1), StaticFriction(0, 6), DynamicFriction(0, 4) { }

    constexpr RigidBodyData(const Fixed16_16& mass, const Fixed16_16& restitution, const Fixed16_16& area, const Fixed16_16& inertia, const Fixed16_16& staticFriction, const Fixed16_16& dynamicFriction)
        : Velocity(0, 0),
          AngularVelocity(0, 0),
          Force(0, 0),
          InverseMass(1 / mass),
          Restitution(restitution),
          Area(area),
          InverseInertia(1 / inertia),
          StaticFriction(staticFriction),
          DynamicFriction(dynamicFriction) { }

    constexpr static RigidBodyData CreateCircleRigidBody(const Fixed16_16 radius, const Fixed16_16& density, const Fixed16_16& restitution, const Fixed16_16& staticFriction, const Fixed16_16& dynamicFriction)
    {
        Fixed16_16 area = radius * radius * Fixed16_16::pi();
        assert(area > Fixed16_16(0) && density > Fixed16_16(0) && restitution >= Fixed16_16(0) && restitution <= Fixed16_16(1) && "Invalid properties of rigidBody");
        Fixed16_16 mass = area * density;

        return RigidBodyData {mass, restitution, area, GetRotationalInertiaCircle(mass, radius), staticFriction, dynamicFriction };
    }

    constexpr static RigidBodyData CreateBoxRigidBody(const Fixed16_16 width, const Fixed16_16 height, const Fixed16_16& density, const Fixed16_16& restitution, const Fixed16_16& staticFriction, const Fixed16_16& dynamicFriction)
    {
        Fixed16_16 area = width * height;
        assert(area > Fixed16_16(0) && density > Fixed16_16(0) && restitution >= Fixed16_16(0) && restitution <= Fixed16_16(1) && "Invalid properties of rigidBody");
        Fixed16_16 mass = area * density;

        return RigidBodyData {area * density, restitution, area, GetRotationalInertiaBox(mass, width, height), staticFriction, dynamicFriction};
    }

    void ApplyForce(const Vector2& direction)
    {
        Force += direction;
    }

    [[nodiscard]] Fixed16_16 GetMass() const
    {
        return 1 / InverseMass;
    }


    static constexpr Fixed16_16 GetRotationalInertiaCircle(const Fixed16_16& mass, const Fixed16_16& radius)
    {
        return Fixed16_16(1) / Fixed16_16(2) * mass * radius * radius;
    }

    static constexpr Fixed16_16 GetRotationalInertiaBox(const Fixed16_16& mass, const Fixed16_16& width, const Fixed16_16& height)
    {
        return Fixed16_16(1) / Fixed16_16(12) * mass * (width * width + height * height);
    }
};