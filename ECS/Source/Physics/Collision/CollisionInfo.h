#pragma once
#include "../../Math/FixedTypes.h"

struct CollisionInfo
{
    Vector2 Normal;
    Entity Entity1;
    Entity Entity2;
    Fixed16_16 Depth;

    Vector2 Contact1;
    Vector2 Contact2;
    int ContactCount;

    bool IsDynamic1;
    bool IsDynamic2;

    CollisionInfo(): Normal(Vector2::Zero()), Entity1(0), Entity2(0), Depth(0), Contact1(Vector2::Zero()), Contact2(Vector2::Zero()), ContactCount(0), IsDynamic1(false), IsDynamic2(false) { }
    CollisionInfo(Vector2 normal, Entity owner, Entity other, Fixed16_16 depth, Vector2 contact1, Vector2 contact2, int contactCount, bool isDynamic1, bool isDynamic2) :
        Normal(normal), Entity1(owner), Entity2(other), Depth(depth), Contact1(contact1), Contact2(contact2), ContactCount(contactCount), IsDynamic1(isDynamic1), IsDynamic2(isDynamic2) { }
};
