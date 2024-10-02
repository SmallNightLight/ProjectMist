#pragma once


struct Lifetime
{
    float CurrentLifetime;

    Lifetime() : CurrentLifetime(0.0f) { }
    explicit Lifetime(const float lifetime) : CurrentLifetime(lifetime) { }
};