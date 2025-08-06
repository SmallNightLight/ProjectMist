#pragma once

#include "../../Math/FixedTypes.h"
#include "../../Math/HashUtils.h"
#include "../../ECS/ECSSettings.h"

struct CollisionPairData
{
    Entity Entity1;
    Entity Entity2;

    Vector2 Position1;
    Vector2 Position2;

    Fixed16_16 Rotation1;
    Fixed16_16 Rotation2;

    uint32_t Hash;

    inline CollisionPairData() = default;

    inline explicit CollisionPairData(Entity entity1, Entity entity2, Vector2 position1, Vector2 position2, Fixed16_16 rotation1, Fixed16_16 rotation2, uint32_t hash1, uint32_t hash2) :
        Entity1(entity1),
        Entity2(entity2),
        Position1(position1),
        Position2(position2),
        Rotation1(rotation1),
        Rotation2(rotation2)
    {
        Hash = CombineHash(hash1, hash2);
    }

    bool operator==(const CollisionPairData& otherCollisionPairData) const
    {
        return
            Entity1 == otherCollisionPairData.Entity1 &&
            Entity2 == otherCollisionPairData.Entity2 &&
            Position1 == otherCollisionPairData.Position1 &&
            Position2 == otherCollisionPairData.Position2 &&
            Rotation1 == otherCollisionPairData.Rotation1 &&
            Rotation2 == otherCollisionPairData.Rotation2;
    }

    bool operator!=(const CollisionPairData& otherCollisionPairData) const
    {
        return !(*this == otherCollisionPairData);
    }
};

struct CollisionPairDataHash
{
    inline std::size_t operator()(const CollisionPairData& collisionPairData) const
    {
        return collisionPairData.Hash;
    }
};