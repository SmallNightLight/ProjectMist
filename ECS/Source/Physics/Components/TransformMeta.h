#pragma once

#include "../../Math/FixedTypes.h"
#include "../Additional/ColliderType.h"
#include "../Additional/RigidBodyType.h"

struct TransformMeta
{
    ColliderType Shape;
    bool IsStatic;
    bool IsKinematic;
    bool IsDynamic;

    bool Active;
    AABB BoundingBox;

    inline TransformMeta() noexcept = default;

    constexpr inline explicit TransformMeta(ColliderType shape, RigidBodyType type) :
      Shape(shape),
      IsStatic(type == Static),
      IsKinematic(type == Kinematic),
      IsDynamic(type == Dynamic),
      Active(true),
      BoundingBox(Vector2(0, 0), Vector2(0, 0)) { }

    inline explicit TransformMeta(Stream& stream)
    {
        //Read object data
        Shape = stream.ReadEnum<ColliderType, uint8_t>();

        //Read rigidBody type
        IsStatic = stream.ReadBool();
        IsKinematic = stream.ReadBool();
        IsDynamic = stream.ReadBool();

        Active = true;
        BoundingBox = AABB(Vector2(0, 0), Vector2(0, 0));
    }

    void Serialize(Stream& stream) const
    {
        //Write meta data
        stream.WriteEnum<ColliderType, uint8_t>(Shape);
        stream.WriteBool(IsStatic);
        stream.WriteBool(IsKinematic);
        stream.WriteBool(IsDynamic);
    }
};