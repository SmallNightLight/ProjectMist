#pragma once

struct Lifetime
{
    Fixed16_16 CurrentLifetime;

    inline Lifetime() noexcept = default;
    constexpr inline explicit Lifetime(const Fixed16_16 lifetime) : CurrentLifetime(lifetime) { }
};