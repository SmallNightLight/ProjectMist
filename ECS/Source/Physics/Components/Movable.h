#pragma once

#include "../../Math/FixedTypes.h"
#include "../../Math/Stream.h"

struct Movable
{
    Fixed16_16 Speed;
    Movable() : Speed(1) { }
    explicit Movable(Fixed16_16 speed) : Speed(speed) { }
    explicit Movable(Stream& stream)
    {
        Speed = stream.ReadFixed();
    }

    void Serialize(Stream& stream) const
    {
        stream.WriteFixed(Speed);
    }
};