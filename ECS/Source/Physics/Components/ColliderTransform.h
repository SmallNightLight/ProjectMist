#pragma once

#include "../../Math/FixedTypes.h"

#include "BoxCollider.h"
#include "CircleCollider.h"

enum ColliderType
{
    Circle,
    Box,
    Polygon
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

    ColliderTransform() : Position(0, 0), Rotation(0), Shape(Circle), IsStatic(true), IsKinematic(false), IsDynamic(false), TransformUpdateRequired(false), AABBUpdateRequired(false) { }

    constexpr ColliderTransform(Vector2 position, Fixed16_16 rotation, ColliderType shape, RigidBodyType type) : Position(position), Rotation(rotation), Shape(shape), IsStatic(type == Static),
      IsKinematic(type == Kinematic),
      IsDynamic(type == Dynamic), TransformUpdateRequired(true), AABBUpdateRequired(true)
    {
    }

    constexpr void SetRigidBodyType(RigidBodyType type)
    {
        IsStatic = type == Static;
        IsKinematic = type == Kinematic;
        IsDynamic = type == Dynamic;
    }

    void MovePosition(Vector2 direction)
    {
        Position += direction;
        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
    }

    void SetPosition(Vector2 position)
    {
        Position = position;
        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
    }

    void Rotate(Fixed16_16 amount)
    {
        Rotation += amount;
        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
    }

    const std::vector<Vector2>& GetTransformedVertices(std::vector<Vector2>& transformedVertices, std::vector<Vector2>& vertices)
    {
        if (TransformUpdateRequired)
        {
            for (int i = 0; i < transformedVertices.size(); ++i)
            {
                transformedVertices[i] = Transform(vertices[i]);
            }

            TransformUpdateRequired = false;
        }

        return transformedVertices;
    }

    const AABB& GetAABB(const Fixed16_16& radius)
    {
        if (AABBUpdateRequired)
        {
            BoundingBox = AABB{Vector2(Position.X - radius, Position.Y - radius), Vector2(Position.X + radius, Position.Y + radius)};
            AABBUpdateRequired = false;
        }

        return BoundingBox;
    }

    const AABB& GetAABB(std::vector<Vector2>& transformedVertices, std::vector<Vector2>& vertices)
    {
        if (AABBUpdateRequired)
        {
            Fixed16_16 minX = std::numeric_limits<Fixed16_16>::max();
            Fixed16_16 minY = std::numeric_limits<Fixed16_16>::max();;
            Fixed16_16 maxX = std::numeric_limits<Fixed16_16>::min();;
            Fixed16_16 maxY = std::numeric_limits<Fixed16_16>::min();;

            for(Vector2 vertex : GetTransformedVertices(transformedVertices, vertices))
            {
                if (vertex.X < minX) { minX = vertex.X; }
                if (vertex.X > maxX) { maxX = vertex.X; }
                if (vertex.Y < minY) { minY = vertex.Y; }
                if (vertex.Y > maxY) { maxY = vertex.Y; }
            }

            BoundingBox = AABB{Vector2(minX, minY), Vector2(maxX, maxY)};
            AABBUpdateRequired = false;
        }

        return BoundingBox;
    }

    [[nodiscard]] Vector2 Transform (Vector2 vector) const
    {
        Fixed16_16 sin = fpm::sin(Rotation);
        Fixed16_16 cos = fpm::cos(Rotation);

        return Vector2{cos * vector.X - sin * vector.Y + Position.X, sin * vector.X + cos * vector.Y + Position.Y};
    }

private:
    AABB BoundingBox;

    bool TransformUpdateRequired;
    bool AABBUpdateRequired;
};