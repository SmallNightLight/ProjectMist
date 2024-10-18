#pragma once

#include "../Math/FixedVector2.h"

struct Lifetime
{
    Fixed16_16 CurrentLifetime;

    Lifetime() : CurrentLifetime(0) { }
    explicit Lifetime(const Fixed16_16 lifetime) : CurrentLifetime(lifetime) { }
};