#pragma once

#include "../../Math/FixedTypes.h"

struct CollisionPairData
{
    EntityPair EntityKey;

    Vector2 Position1;
    Vector2 Position2;

    Fixed16_16 Rotation1;
    Fixed16_16 Rotation2;

    bool Collision;

    inline CollisionPairData() noexcept = default;

    inline explicit CollisionPairData(Entity entity1, Entity entity2, Vector2 position1, Vector2 position2, Fixed16_16 rotation1, Fixed16_16 rotation2, bool collision = false) :
        EntityKey(EntityPair::Make(entity1, entity2)),
        Position1(position1),
        Position2(position2),
        Rotation1(rotation1),
        Rotation2(rotation2),
        Collision(collision) { }

    inline constexpr bool operator==(const CollisionPairData& other) const noexcept
    {
        return EntityKey == other.EntityKey;
    }

    inline constexpr bool operator<(const CollisionPairData& other) const noexcept
    {
        return EntityKey < other.EntityKey;
    }

    bool IsDataEqual(const CollisionPairData& otherCollisionPairData) const
    {
        return
            Position1 == otherCollisionPairData.Position1 &&
            Position2 == otherCollisionPairData.Position2 &&
            Rotation1 == otherCollisionPairData.Rotation1 &&
            Rotation2 == otherCollisionPairData.Rotation2;
    }
};