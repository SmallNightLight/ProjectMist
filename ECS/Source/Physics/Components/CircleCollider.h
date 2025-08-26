#pragma once

#include "../../Math/FixedTypes.h"
#include "../../Math/Stream.h"
#include "Transform.h"
#include "TransformMeta.h"

class CircleCollider
{
public:
    inline CircleCollider() noexcept = default;

    constexpr inline explicit CircleCollider(Fixed16_16 _radius) : Radius(_radius) { }

    inline explicit CircleCollider(Stream& stream)
    {
        Radius = stream.ReadFixed();
    }

    inline constexpr Fixed16_16 GetRadius() const
    {
        return Radius;
    }

    const AABB& GetAABB(Transform& transform, TransformMeta& transformMeta)
    {
        if (transform.AABBUpdateRequired)
        {
            transformMeta.BoundingBox = AABB(Vector2(transform.Position.X - Radius, transform.Position.Y - Radius), Vector2(transform.Position.X + Radius, transform.Position.Y + Radius));
            transform.AABBUpdateRequired = false;
        }

        return transformMeta.BoundingBox;
    }

    void Serialize(Stream& stream) const
    {
          stream.WriteFixed(Radius);
    }

private:
    Fixed16_16 Radius;
};