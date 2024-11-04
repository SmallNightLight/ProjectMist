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

    static RigidBodyData CreatePolygonRigidBody(const std::vector<Vector2>& vertices, const Fixed16_16& density, const Fixed16_16& restitution, const Fixed16_16& staticFriction, const Fixed16_16& dynamicFriction)
    {
        Fixed16_16 area = GetPolygonArea(vertices);
        assert(area > Fixed16_16(0) && density > Fixed16_16(0) && restitution >= Fixed16_16(0) && restitution <= Fixed16_16(1) && "Invalid properties of rigidBody");
        Fixed16_16 mass = area * density;

        return RigidBodyData {area * density, restitution, area, GetRotationalInertiaPolygon(mass, vertices), staticFriction, dynamicFriction};
    }

    void ApplyForce(const Vector2& direction)
    {
        Force += direction;
    }

    [[nodiscard]] Fixed16_16 GetMass() const
    {
        return 1 / InverseMass;
    }

    static Fixed16_16 GetPolygonArea(const std::vector<Vector2>& vertices) //TODO: Make it constexpr
    {
        if (vertices.size() < 3)
        {
            //Area is zero for invalid polygons
            return Fixed16_16(0);
        }

        Fixed16_16 area(0);
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            const Vector2& current = vertices[i];
            const Vector2& next = vertices[(i + 1) % vertices.size()];

            area += (current.X * next.Y) - (current.Y * next.X);
        }

        return abs(area) / Fixed16_16(2);;
    }

    static constexpr Fixed16_16 GetRotationalInertiaCircle(const Fixed16_16& mass, const Fixed16_16& radius)
    {
        return Fixed16_16(1) / Fixed16_16(2) * mass * radius * radius;
    }

    static constexpr Fixed16_16 GetRotationalInertiaBox(const Fixed16_16& mass, const Fixed16_16& width, const Fixed16_16& height)
    {
        return Fixed16_16(1) / Fixed16_16(12) * mass * (width * width + height * height);
    }

    static Fixed16_16 GetRotationalInertiaPolygon(const Fixed16_16& mass, const std::vector<Vector2>& vertices) //TODO: STRAIGHT FROM CHATGPT, NOT GUARANTEED TO WORK
    {
        if (vertices.size() < 3)
        {
            return Fixed16_16(0);
        }

        //Step 1: Calculate the centroid of the polygon
        Fixed16_16 centroidX(0);
        Fixed16_16 centroidY(0);
        Fixed16_16 area = Fixed16_16(0);

        for (size_t i = 0; i < vertices.size(); ++i)
        {
            const Vector2& current = vertices[i];
            const Vector2& next = vertices[(i + 1) % vertices.size()];

            Fixed16_16 crossProduct = (current.X * next.Y - current.Y * next.X);
            centroidX += (current.X + next.X) * crossProduct;
            centroidY += (current.Y + next.Y) * crossProduct;
            area += crossProduct;
        }

        area /= Fixed16_16(2);
        Fixed16_16 inverseArea6 = Fixed16_16(1) / (Fixed16_16(6) * area);
        centroidX *= inverseArea6;
        centroidY *= inverseArea6;

        //Step 2: Calculate the moment of inertia about the centroid
        Fixed16_16 inertia = Fixed16_16(0);

        for (size_t i = 0; i < vertices.size(); ++i)
        {
            const Vector2& current = vertices[i];
            const Vector2& next = vertices[(i + 1) % vertices.size()];

            Fixed16_16 crossProduct = (current.X * next.Y - current.Y * next.X);

            Fixed16_16 xSquaredSum = (current.X * current.X + current.X * next.X + next.X * next.X);
            Fixed16_16 ySquaredSum = (current.Y * current.Y + current.Y * next.Y + next.Y * next.Y);

            inertia += crossProduct * (xSquaredSum + ySquaredSum);
        }

        inertia = abs(inertia) / (Fixed16_16(12) * area);
        return mass * inertia;
    }
};