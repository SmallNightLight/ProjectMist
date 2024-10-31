#pragma once

#include "../../Math/FixedTypes.h"

struct Movable
{
    Fixed16_16 Speed;
    Movable() : Speed(1) { }
    explicit Movable(Fixed16_16 speed) : Speed(speed) { }
};
