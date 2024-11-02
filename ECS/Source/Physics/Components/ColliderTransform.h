#pragma once

#include "../../Math/FixedTypes.h"

enum ColliderType
{
    Circle,
    Box,
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

    bool TransformUpdateRequired;

    ColliderTransform() : Position(0, 0), Rotation(0), Shape(ColliderType::Box), IsStatic(true), IsKinematic(false), IsDynamic(false), TransformUpdateRequired(false) { }

    constexpr ColliderTransform(Vector2 position, Fixed16_16 rotation, ColliderType shape, RigidBodyType type) : Position(position), Rotation(rotation), Shape(shape), IsStatic(type == RigidBodyType::Static),
      IsKinematic(type == RigidBodyType::Kinematic),
      IsDynamic(type == RigidBodyType::Dynamic), TransformUpdateRequired(true)
    {
    }

    constexpr inline void SetRigidBodyType(RigidBodyType type)
    {
        IsStatic = type == RigidBodyType::Static;
        IsKinematic = type == RigidBodyType::Kinematic;
        IsDynamic = type == RigidBodyType::Dynamic;
    }


    void MovePosition(Vector2 direction)
    {
        Position += direction;
        TransformUpdateRequired = true;
    }

    void SetPosition(Vector2 position)
    {
        Position = position;
        TransformUpdateRequired = true;
    }

    void Rotate(Fixed16_16 amount)
    {
        Rotation += amount;
        TransformUpdateRequired = true;
    }

    [[nodiscard]] Vector2 Transform (Vector2 vector) const
    {
        Fixed16_16 sin = fpm::sin(Rotation);
        Fixed16_16 cos = fpm::cos(Rotation);

        return Vector2{cos * vector.X - sin * vector.Y + Position.X, sin * vector.X + cos * vector.Y + Position.Y};
    }
};