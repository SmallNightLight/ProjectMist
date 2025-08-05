#pragma once

#include "../../Math/FixedTypes.h"
#include "../../ECS/ECSSettings.h"

struct CollisionCheckInfo
{
    Entity Entity1;
    Entity Entity2;

    Vector2 Velocity1;
    Vector2 Velocity2;

    Fixed16_16 AngularVelocity1;
    Fixed16_16 AngularVelocity2;

    uint8_t GravityScale1;
    uint8_t GravityScale2;

    uint8_t MassScale1;
    uint8_t MassScale2;

    uint32_t Hash;

    inline constexpr CollisionCheckInfo() :
        Entity1(0),
        Entity2(0),
        Velocity1(0, 0),
        Velocity2(0, 0),
        AngularVelocity1(0),
        AngularVelocity2(0),
        GravityScale1(0),
        GravityScale2(0),
        MassScale1(0),
        MassScale2(0),
        Hash(0) { }

    CollisionCheckInfo(Entity entity1, Entity entity2, Vector2 velocity1,Vector2 velocity2, Fixed16_16 angularVelocity1, Fixed16_16 angularVelocity2, uint8_t gravityScale1, uint8_t gravityScale2, uint8_t massScale1, uint8_t massScale2) :
        Entity1(entity1),
        Entity2(entity2),
        Velocity1(velocity1),
        Velocity2(velocity2),
        AngularVelocity1(angularVelocity1),
        AngularVelocity2(angularVelocity2),
        GravityScale1(gravityScale1),
        GravityScale2(gravityScale2),
        MassScale1(massScale1),
        MassScale2(massScale2)
    {
        //Calculate hash
        CalculateHash();
    }

    inline void CalculateHash()
    {
        Hash = CombineHash(static_cast<uint32_t>(Entity1), static_cast<uint32_t>(Entity2));
        Hash = CombineHash(Hash, static_cast<uint32_t>(Velocity1.X.raw_value()));
        Hash = CombineHash(Hash, static_cast<uint32_t>(Velocity1.Y.raw_value()));
        Hash = CombineHash(Hash, static_cast<uint32_t>(Velocity2.X.raw_value()));
        Hash = CombineHash(Hash, static_cast<uint32_t>(Velocity2.Y.raw_value()));
        Hash = CombineHash(Hash, static_cast<uint32_t>(AngularVelocity1.raw_value()));
        Hash = CombineHash(Hash, static_cast<uint32_t>(AngularVelocity2.raw_value()));
        Hash = CombineHash(Hash, static_cast<uint32_t>(GravityScale1));
        Hash = CombineHash(Hash, static_cast<uint32_t>(GravityScale2));
        Hash = CombineHash(Hash, static_cast<uint32_t>(MassScale1));
        Hash = CombineHash(Hash, static_cast<uint32_t>(MassScale2));
    }

    bool operator==(const CollisionCheckInfo& otherCollisionCheckInfo) const
    {
        return
            Entity1 == otherCollisionCheckInfo.Entity1 &&
            Entity2 == otherCollisionCheckInfo.Entity2 &&
            Velocity1 == otherCollisionCheckInfo.Velocity1 &&
            Velocity2 == otherCollisionCheckInfo.Velocity2 &&
            AngularVelocity1 == otherCollisionCheckInfo.AngularVelocity1 &&
            AngularVelocity2 == otherCollisionCheckInfo.AngularVelocity2 &&
            GravityScale1 == otherCollisionCheckInfo.GravityScale1 &&
            GravityScale2 == otherCollisionCheckInfo.GravityScale2 &&
            MassScale1 == otherCollisionCheckInfo.MassScale1 &&
            MassScale2 == otherCollisionCheckInfo.MassScale2;

    }

    bool operator!=(const CollisionCheckInfo& otherCollisionCheckInfo) const
    {
        return !(*this == otherCollisionCheckInfo);
    }
};

struct CollisionCheckInfoHash
{
    inline std::size_t operator()( const CollisionCheckInfo& checkInfo ) const
    {
        return checkInfo.Hash;
    }
};