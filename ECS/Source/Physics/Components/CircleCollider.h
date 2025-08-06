#pragma once

#include "../../Math/FixedTypes.h"
#include "../../Math/Stream.h"

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

    void Serialize(Stream& stream) const
    {
          stream.WriteFixed(Radius);
    }

private:
    Fixed16_16 Radius;
};