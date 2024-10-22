#pragma once

#include <iostream>
#include <ostream>

#include "../../Math/FixedTypes.h"

enum ColliderType
{
    Box,
    Circle,
    Convex
};

enum RigidBodyType
{
    Static,
    Kinematic,
    Dynamic
};

struct ColliderTransform
{
    Vector2 Position;
    Fixed16_16 Rotation;
    ColliderType Shape;
    bool IsStatic;
    bool IsKinematic;
    bool IsDynamic;

    ColliderTransform() : Position(0, 0), Rotation(0), Shape(ColliderType::Box), IsStatic(true), IsKinematic(false), IsDynamic(false) { }

    constexpr ColliderTransform(Vector2 position, Fixed16_16 rotation, ColliderType shape, RigidBodyType type) : Position(position), Rotation(rotation), Shape(shape), IsStatic(type == RigidBodyType::Static),
      IsKinematic(type == RigidBodyType::Kinematic),
      IsDynamic(type == RigidBodyType::Dynamic)
    { }

    constexpr inline void SetRigidBodyType(RigidBodyType type)
    {
        IsStatic = type == RigidBodyType::Static;
        IsKinematic = type == RigidBodyType::Kinematic;
        IsDynamic = type == RigidBodyType::Dynamic;
    }
};