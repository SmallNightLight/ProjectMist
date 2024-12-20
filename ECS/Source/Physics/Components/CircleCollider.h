#pragma once

#include "../../Math/FixedTypes.h"
#include "../../Math/Stream.h"

struct CircleCollider
{
    Fixed16_16 Radius;

    CircleCollider() : Radius(0) { }
    explicit CircleCollider(Fixed16_16 _radius) : Radius(_radius) { }
    explicit CircleCollider(Stream& stream)
    {
        Radius = stream.ReadFixed();
    }

    void Serialize(Stream& stream) const
    {
          stream.WriteFixed(Radius);
    }
};