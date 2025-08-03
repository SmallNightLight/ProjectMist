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

    CollisionCheckInfo() = default;

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
        MassScale2(massScale2) { }

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
    std::size_t operator()( const CollisionCheckInfo& checkInfo ) const
    {
        uint32_t hash = CombineHash(static_cast<uint32_t>(checkInfo.Entity1), static_cast<uint32_t>(checkInfo.Entity2));
        hash = CombineHash(hash, static_cast<uint32_t>(checkInfo.Velocity1.X.raw_value()));
        hash = CombineHash(hash, static_cast<uint32_t>(checkInfo.Velocity1.Y.raw_value()));
        hash = CombineHash(hash, static_cast<uint32_t>(checkInfo.Velocity2.X.raw_value()));
        hash = CombineHash(hash, static_cast<uint32_t>(checkInfo.Velocity2.Y.raw_value()));
        hash = CombineHash(hash, static_cast<uint32_t>(checkInfo.AngularVelocity1.raw_value()));
        hash = CombineHash(hash, static_cast<uint32_t>(checkInfo.AngularVelocity2.raw_value()));
        hash = CombineHash(hash, static_cast<uint32_t>(checkInfo.GravityScale1));
        hash = CombineHash(hash, static_cast<uint32_t>(checkInfo.GravityScale2));
        hash = CombineHash(hash, static_cast<uint32_t>(checkInfo.MassScale1));
        hash = CombineHash(hash, static_cast<uint32_t>(checkInfo.MassScale2));

        return static_cast<std::size_t>(hash);
    }
};