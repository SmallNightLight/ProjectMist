#pragma once

#include "../../Math/FixedTypes.h"
#include "../../ECS/ECSSettings.h"

struct CollisionCheckInfo
{
    Entity Entity1;
    Entity Entity2;

    uint32_t Hash;


    bool operator==(const CollisionCheckInfo& otherCollisionCheckInfo) const
    {
        return
                Entity1 == otherCollisionCheckInfo.Entity1 &&
                Entity2 == otherCollisionCheckInfo.Entity2;
    }

    bool operator!=(const CollisionCheckInfo& otherCollisionCheckInfo) const
    {
        return !(*this == otherCollisionCheckInfo);
    }
};