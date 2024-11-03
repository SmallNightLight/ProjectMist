#pragma once

#include "../../Math/FixedTypes.h"

struct CircleCollider
{
    Fixed16_16 Radius;

    CircleCollider() : Radius(0) { }
    explicit CircleCollider(Fixed16_16 _radius) : Radius(_radius) { }
};