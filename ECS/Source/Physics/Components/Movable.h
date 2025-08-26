#pragma once

#include "../../Math/FixedTypes.h"

struct Movable
{
    Fixed16_16 Speed;

    inline Movable() noexcept = default;

    inline constexpr explicit Movable(Fixed16_16 speed) : Speed(speed) { }

    inline explicit Movable(Stream& stream)
    {
        Speed = stream.ReadFixed();
    }

    void Serialize(Stream& stream) const
    {
        stream.WriteFixed(Speed);
    }
};