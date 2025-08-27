#pragma once

#include "../../Math/FixedTypes.h"
#include "../../Math/HashUtils.h"

#include <cassert>

struct RigidBodyBaseData
{
    Vector2 Velocity;
    Fixed16_16 AngularVelocity;
    uint8_t GravityScale;
    uint8_t MassScale;
};

struct RigidBodyKey
{
    uint64_t Key1;
    uint64_t Key2;

    inline constexpr bool operator==(const TransformKey& other) const noexcept
    {
        return Key1 == other.Key1 && Key2 == other.Key2;
    }

    inline constexpr bool operator!=(const TransformKey& other) const noexcept
    {
        return Key1 != other.Key1 || Key2 != other.Key2;
    }
};

struct RigidBodyData
{
    union
    {
        RigidBodyBaseData Base;
        RigidBodyKey Key;
    };

    Vector2 Force; //Temporary container for force, reset to 0 in rigidBody after use //TODO: might not work cor with hashing when changed

    Fixed16_16 InverseMass;
    Fixed16_16 Restitution;
    Fixed16_16 InverseInertia;
    Fixed16_16 StaticFriction; //TODO: add materials and only save the index instead of he entire friction value (Restitution, StaticFriction, DynamicFriction)
    Fixed16_16 DynamicFriction;

    bool Changed;   //Non-persistent flag for caching

public:
    inline RigidBodyData() noexcept = default;

    constexpr inline explicit RigidBodyData(const Fixed16_16& mass, const Fixed16_16& restitution, const Fixed16_16& inertia, const Fixed16_16& staticFriction, const Fixed16_16& dynamicFriction) :
        Base{ Vector2(0,0), Fixed16_16(0), 1, 1 },
        Force(0, 0),
        InverseMass(1 / mass),
        Restitution(restitution),
        InverseInertia(1 / inertia),
        StaticFriction(staticFriction),
        DynamicFriction(dynamicFriction) { }

    constexpr inline explicit RigidBodyData(const Fixed16_16& staticFriction, const Fixed16_16& dynamicFriction) :
    Base{ Vector2(0,0), Fixed16_16(0), 0, 0 },
        Force(0, 0),
        InverseMass(0),
        Restitution(0),
        InverseInertia(0),
        StaticFriction(staticFriction),
        DynamicFriction(dynamicFriction) { }

    inline explicit RigidBodyData(Stream& stream)
    {
        Base.Velocity = stream.ReadVector2();
        Base.AngularVelocity = stream.ReadFixed();
        Base.GravityScale = stream.ReadInteger<uint8_t>();
        Base.MassScale = stream.ReadInteger<uint8_t>();

        Force = stream.ReadVector2();
        InverseMass = stream.ReadFixed();
        Restitution = stream.ReadFixed();
        InverseInertia = stream.ReadFixed();
        StaticFriction = stream.ReadFixed();
        DynamicFriction = stream.ReadFixed();
    }

    constexpr static RigidBodyData CreateStaticRigidBody(const Fixed16_16& staticFriction, const Fixed16_16& dynamicFriction)
    {
        return RigidBodyData(staticFriction, dynamicFriction);
    }

    constexpr static RigidBodyData CreateCircleRigidBody(const Fixed16_16 radius, const Fixed16_16& density, const Fixed16_16& restitution, const Fixed16_16& staticFriction, const Fixed16_16& dynamicFriction)
    {
        Fixed16_16 area = radius * radius * Fixed16_16::pi();
        assert(area > Fixed16_16(0) && density > Fixed16_16(0) && restitution >= Fixed16_16(0) && restitution <= Fixed16_16(1) && "Invalid properties of rigidBody");
        Fixed16_16 mass = area * density;

        return RigidBodyData(mass, restitution, GetRotationalInertiaCircle(mass, radius), staticFriction, dynamicFriction);
    }

    constexpr static RigidBodyData CreateBoxRigidBody(const Fixed16_16 width, const Fixed16_16 height, const Fixed16_16& density, const Fixed16_16& restitution, const Fixed16_16& staticFriction, const Fixed16_16& dynamicFriction)
    {
        Fixed16_16 area = width * height;
        assert(area > Fixed16_16(0) && density > Fixed16_16(0) && restitution >= Fixed16_16(0) && restitution <= Fixed16_16(1) && "Invalid properties of rigidBody");
        Fixed16_16 mass = area * density;

        return RigidBodyData(area * density, restitution, GetRotationalInertiaBox(mass, width, height), staticFriction, dynamicFriction);
    }

    static RigidBodyData CreatePolygonRigidBody(const std::vector<Vector2>& vertices, const Fixed16_16& density, const Fixed16_16& restitution, const Fixed16_16& staticFriction, const Fixed16_16& dynamicFriction)
    {
        Fixed16_16 area = GetPolygonArea(vertices);
        assert(area > Fixed16_16(0) && density > Fixed16_16(0) && restitution >= Fixed16_16(0) && restitution <= Fixed16_16(1) && "Invalid properties of rigidBody");
        Fixed16_16 mass = area * density;

        return RigidBodyData(area * density, restitution, GetRotationalInertiaPolygon(mass, vertices), staticFriction, dynamicFriction);
    }

    inline void ApplyForce(const Vector2& direction)
    {
        Force += direction;
    }

    inline void UpdateGravityScale(uint8_t newGravityScale)
    {
        Base.GravityScale = newGravityScale;
    }

    inline void UpdateMassScale(uint8_t newMassScale)
    {
        Base.GravityScale = newMassScale;
    }

    inline Fixed16_16 Mass() const
    {
        return 1 / InverseMass;
    }

    inline Fixed16_16 Inertia() const
    {
        return 1 / InverseInertia;
    }

    static Fixed16_16 GetPolygonArea(const std::vector<Vector2>& vertices)
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
        return mass * radius * radius / Fixed16_16(2);
    }

    static constexpr Fixed16_16 GetRotationalInertiaBox(const Fixed16_16& mass, const Fixed16_16& width, const Fixed16_16& height)
    {
        return mass * (width * width + height * height) / Fixed16_16(12);
    }

    static Fixed16_16 GetRotationalInertiaPolygon(const Fixed16_16& mass, const std::vector<Vector2>& vertices) //TODO: STRAIGHT FROM CHATGPT, NOT GUARANTEED TO WORK -edit: works!
    {
        if (vertices.size() < 3)
        {
            return Fixed16_16(0);
        }

        //Calculate the centroid of the polygon
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

        //Calculate the moment of inertia about the centroid
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

    void Serialize(Stream& stream) const
    {
        stream.WriteVector2(Base.Velocity);
        stream.WriteFixed(Base.AngularVelocity);
        stream.WriteInteger(Base.GravityScale);
        stream.WriteInteger(Base.MassScale);

        stream.WriteVector2(Force);
        stream.WriteFixed(InverseMass);
        stream.WriteFixed(Restitution);
        stream.WriteFixed(InverseInertia);
        stream.WriteFixed(StaticFriction);
        stream.WriteFixed(DynamicFriction);
    }
};