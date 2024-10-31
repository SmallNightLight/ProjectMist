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
    Fixed16_16 Sin;
    Fixed16_16 Cos;

    ColliderType Shape;
    bool IsStatic;
    bool IsKinematic;
    bool IsDynamic;

    ColliderTransform() : Position(0, 0), Rotation(0), Sin(0), Cos(0), Shape(ColliderType::Box), IsStatic(true), IsKinematic(false), IsDynamic(false) { }

    constexpr ColliderTransform(Vector2 position, Fixed16_16 rotation, ColliderType shape, RigidBodyType type) : Position(position), Rotation(rotation), Sin(fpm::sin(rotation)), Cos(fpm::cos(rotation)), Shape(shape), IsStatic(type == RigidBodyType::Static),
      IsKinematic(type == RigidBodyType::Kinematic),
      IsDynamic(type == RigidBodyType::Dynamic)
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
    }

    void SetPosition(Vector2 position)
    {
        Position = position;
    }

    void Rotate(Fixed16_16 amount)
    {
        Rotation += amount;
    }

    [[nodiscard]] Vector2 Transform (Vector2 vector) const
    {
        return Vector2{Cos * vector.X - Sin * vector.Y + Position.X, Sin * vector.X + Cos * vector.Y + Position.Y};
    }
};