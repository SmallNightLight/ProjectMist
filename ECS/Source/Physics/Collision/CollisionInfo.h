#pragma once
#include "../../Math/FixedTypes.h"

struct CollisionInfo
{
    Vector2 Normal;
    Entity Owner;
    Entity Other;
    //Fixed16_16 TimOfImpact;
    Fixed16_16 Depth;

    CollisionInfo() = default;
    CollisionInfo(Vector2 normal, Entity owner, Entity other, Fixed16_16 depth) : Normal(normal), Owner(owner), Other(other), Depth(depth) { }
};
