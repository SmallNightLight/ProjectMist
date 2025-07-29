#pragma once

#include "../Enums/HashType.h"
#include "../Enums/HashUpdateType.h"
#include "../../Math/FixedTypes.h"
#include "../../Math/Stream.h"

#include <cassert>

struct RigidBodyData
{
    Vector2 Velocity;
    Fixed16_16 AngularVelocity;
    Vector2 Force; //Temporary container for force, reset to 0 in rigidBody after use

    Fixed16_16 InverseMass;
    Fixed16_16 Restitution;
    Fixed16_16 InverseInertia;
    Fixed16_16 StaticFriction; //TODO: add materials and only save the index instead of he entire friction value (Restitution, StaticFriction, DynamicFriction)
    Fixed16_16 DynamicFriction;

    bool Active;
    Vector2 LastPosition; //TODO: remove this
    Fixed16_16 LastRotation;

private:
    uint32_t Hash;              //The final hash that is used for caching
    uint32_t BaseHash;          //The base hash for variables that do not change often
    //HashType EntityHashType;    //Declares which variables are expected to change often
    HashUpdateType HashUpdateRequired;

public:
    RigidBodyData() : //TODO: Add position and rotation to this class to improve? variables double?
        Velocity(0, 0),
        AngularVelocity(0, 0),
        Force(0, 0), InverseMass(1),
        Restitution(0, 5),
        InverseInertia(1),
        StaticFriction(0, 6),
        DynamicFriction(0, 4),
        Active(true),
        LastPosition(0, 0),
        LastRotation(0),
        Hash(0),
        BaseHash(0),
        //EntityHashType(DynamicHashType),
        HashUpdateRequired(HashUpdateType::FullUpdate) { }

    constexpr RigidBodyData(const Fixed16_16& mass, const Fixed16_16& restitution, const Fixed16_16& area, const Fixed16_16& inertia, const Fixed16_16& staticFriction, const Fixed16_16& dynamicFriction) //, HashType hashType = DynamicHashType)
        : Velocity(0, 0),
          AngularVelocity(0, 0),
          Force(0, 0),
          InverseMass(1 / mass),
          Restitution(restitution),
          InverseInertia(1 / inertia),
          StaticFriction(staticFriction),
          DynamicFriction(dynamicFriction),
          Active(true),
          LastPosition(0, 0),
          LastRotation(0),
          Hash(0),
          BaseHash(0),
          //EntityHashType(hashType),
          HashUpdateRequired(HashUpdateType::FullUpdate) { } //ToDO: No area use?

    explicit RigidBodyData(Stream& stream)
    {
        Velocity = stream.ReadVector2();
        AngularVelocity = stream.ReadFixed();
        Force = stream.ReadVector2();
        InverseMass = stream.ReadFixed();
        Restitution = stream.ReadFixed();
        InverseInertia = stream.ReadFixed();
        StaticFriction = stream.ReadFixed();
        DynamicFriction = stream.ReadFixed();

        Active = true;
        LastPosition = Vector2(0, 0);
        LastRotation = Fixed16_16 (0);

        HashUpdateRequired = HashUpdateType::FullUpdate;
    }

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

    inline void ApplyForce(const Vector2& direction)
    {
        Force += direction;
    }

    inline Fixed16_16 Mass() const
    {
        return 1 / InverseMass;
    }

    inline Fixed16_16 Inertia() const
    {
        return 1 / InverseInertia;
    }

    constexpr inline uint32_t GetHash(Vector2 position, Fixed16_16 rotation)
    {
        if (HashUpdateRequired == HashUpdateType::None) return Hash;

        if (HashUpdateRequired == HashUpdateType::FullUpdate)
        {
            //Recompute base hash
            BaseHash = CombineHash(static_cast<uint32_t>(InverseMass.raw_value()), static_cast<uint32_t>(Restitution.raw_value()));
            BaseHash = CombineHash(BaseHash, static_cast<uint32_t>(InverseInertia.raw_value()));
            BaseHash = CombineHash(BaseHash, static_cast<uint32_t>(StaticFriction.raw_value()));
            BaseHash = CombineHash(BaseHash, static_cast<uint32_t>(DynamicFriction.raw_value()));
        }

        //Recompute final hash
        Hash = CombineHash(BaseHash, static_cast<uint32_t>(position.X.raw_value()));
        Hash = CombineHash(Hash, static_cast<uint32_t>(position.Y.raw_value()));
        Hash = CombineHash(Hash, static_cast<uint32_t>(rotation.raw_value()));
        Hash = CombineHash(Hash, static_cast<uint32_t>(Velocity.X.raw_value()));
        Hash = CombineHash(Hash, static_cast<uint32_t>(Velocity.Y.raw_value()));
        Hash = CombineHash(Hash, static_cast<uint32_t>(AngularVelocity.raw_value()));

        HashUpdateRequired = HashUpdateType::None;
        return Hash;
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
        return Fixed16_16(1) / Fixed16_16(2) * mass * radius * radius;
    }

    static constexpr Fixed16_16 GetRotationalInertiaBox(const Fixed16_16& mass, const Fixed16_16& width, const Fixed16_16& height)
    {
        return Fixed16_16(1) / Fixed16_16(12) * mass * (width * width + height * height);
    }

    static Fixed16_16 GetRotationalInertiaPolygon(const Fixed16_16& mass, const std::vector<Vector2>& vertices) //TODO: STRAIGHT FROM CHATGPT, NOT GUARANTEED TO WORK -edit: works!
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

    void Serialize(Stream& stream) const
    {
        stream.WriteVector2(Velocity);
        stream.WriteFixed(AngularVelocity);
        stream.WriteVector2(Force);
        stream.WriteFixed(InverseMass);
        stream.WriteFixed(Restitution);
        stream.WriteFixed(InverseInertia);
        stream.WriteFixed(StaticFriction);
        stream.WriteFixed(DynamicFriction);
    }
};