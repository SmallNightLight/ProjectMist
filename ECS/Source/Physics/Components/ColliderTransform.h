#pragma once

#include "../../Math/FixedTypes.h"

enum ColliderType{
    Box,
    Circle,
    Convex
};

struct ColliderTransform
{
    Vector2 Position;
    Fixed16_16 Rotation;
    bool IsStatic;
    bool IsKinematic;
    bool IsDynamic;
    ColliderType ColliderType;
};