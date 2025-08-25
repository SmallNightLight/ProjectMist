#pragma once

#include "../../Math/FixedTypes.h"
#include "../../Math/Stream.h"
#include "../../Math/HashUtils.h"

#include "../Additional/ColliderType.h"
#include "../Additional/RigidBodyType.h"

struct ColliderTransform
{
    Vector2 Position;
    Fixed16_16 Rotation; //Radians

    ColliderType Shape;
    bool IsStatic;
    bool IsKinematic;
    bool IsDynamic;

    bool Active;

    inline ColliderTransform() noexcept = default;

    constexpr inline explicit ColliderTransform(Vector2 position, Fixed16_16 rotation, ColliderType shape, RigidBodyType type) :
        Position(position),
        Rotation(rotation),
        Shape(shape),
        IsStatic(type == Static),
        IsKinematic(type == Kinematic),
        IsDynamic(type == Dynamic),
        Active(true),
        BoundingBox(Vector2(0, 0), Vector2(0, 0)),
        Hash(0),
        TransformUpdateRequired(true),
        AABBUpdateRequired(true),
        HashUpdateRequired(true) { }

    inline explicit ColliderTransform(Stream& stream)
    {
        //Read object data
        Position = stream.ReadVector2();
        Rotation = stream.ReadFixed();
        Shape = stream.ReadEnum<ColliderType, uint8_t>();

        //Read rigidBody type
        IsStatic = stream.ReadBool();
        IsKinematic = stream.ReadBool();
        IsDynamic = stream.ReadBool();

        Active = true;

        BoundingBox = AABB(Vector2(0, 0), Vector2(0, 0));
        Hash = 0;

        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
        HashUpdateRequired = true;
    }

    void MovePosition(Vector2 direction)
    {
        Position += direction;
        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
        HashUpdateRequired = true;
    }

    void SetPosition(Vector2 position)
    {
        Position = position;
        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
        HashUpdateRequired = true;
    }

    void Rotate(Fixed16_16 amount)
    {
        Rotation += amount;
        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
        HashUpdateRequired = true;
    }

    void SetRotation(Fixed16_16 angle)
    {
        Rotation = angle;
        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
        HashUpdateRequired = true;
    }

    Vector2Span GetTransformedVertices(Vector2Span transformedVertices, Vector2Span vertices)
    {
        if (TransformUpdateRequired)
        {
            for (uint32_t i = 0; i < transformedVertices.size; ++i)
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
            BoundingBox = AABB(Vector2(Position.X - radius, Position.Y - radius), Vector2(Position.X + radius, Position.Y + radius));
            AABBUpdateRequired = false;
        }

        return BoundingBox;
    }

    const AABB& GetAABB(Vector2Span transformedVertices, Vector2Span vertices)
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

            BoundingBox = AABB(Vector2(minX, minY), Vector2(maxX, maxY));
            AABBUpdateRequired = false;
        }

        return BoundingBox;
    }

    const AABB& GetAABBFromTransformed(Vector2Span transformedVertices)
    {
        if (AABBUpdateRequired)
        {
            Fixed16_16 minX = std::numeric_limits<Fixed16_16>::max();
            Fixed16_16 minY = std::numeric_limits<Fixed16_16>::max();;
            Fixed16_16 maxX = std::numeric_limits<Fixed16_16>::min();;
            Fixed16_16 maxY = std::numeric_limits<Fixed16_16>::min();;

            for(Vector2 vertex : transformedVertices)
            {
                if (vertex.X < minX) { minX = vertex.X; }
                if (vertex.X > maxX) { maxX = vertex.X; }
                if (vertex.Y < minY) { minY = vertex.Y; }
                if (vertex.Y > maxY) { maxY = vertex.Y; }
            }

            BoundingBox = AABB(Vector2(minX, minY), Vector2(maxX, maxY));
            AABBUpdateRequired = false;
        }

        return BoundingBox;
    }

    [[nodiscard]] Vector2 Transform (const Vector2 vector) const
    {
        Fixed16_16 sin = fpm::sin(Rotation);
        Fixed16_16 cos = fpm::cos(Rotation);

        return Vector2(cos * vector.X - sin * vector.Y + Position.X, sin * vector.X + cos * vector.Y + Position.Y);
    }

    uint32_t GetHash(Entity entity)
    {
        if (!HashUpdateRequired) return Hash;

        //Recompute hash
        Hash = CombineHash(static_cast<uint32_t>(Position.X.raw_value()), static_cast<uint32_t>(Position.Y.raw_value()));
        Hash = CombineHash(Hash, static_cast<uint32_t>(Rotation.raw_value()));
        Hash = CombineHash(Hash, static_cast<uint32_t>(entity));

        HashUpdateRequired = false;
        return Hash;
    }

    //Serializes data into the stream, considering that the bounding box and transformed vertices are already updated
    void Serialize(Stream& stream) const
    {
        //Write object data
        stream.WriteVector2(Position);
        stream.WriteFixed(Rotation);
        stream.WriteEnum<ColliderType, uint8_t>(Shape);

        //Write rigidBody type
        stream.WriteBool(IsStatic);
        stream.WriteBool(IsKinematic);
        stream.WriteBool(IsDynamic);

        assert(!TransformUpdateRequired && "TransformUpdateRequired needs to be false for serialization");
    }

    void OverrideTransformUpdateRequire(bool value) //TODO WHY
    {
        TransformUpdateRequired = value;
    }

    void OverrideAABBUpdateRequired(bool value)
    {
        AABBUpdateRequired = value;
    }

private:
    AABB BoundingBox;
    uint32_t Hash;

    bool TransformUpdateRequired;
    bool AABBUpdateRequired;
    bool HashUpdateRequired;
};

//ToDo: Combine TransformUpdateRequired and AABBUpdateRequired