#pragma once

#include <cstdint>

struct ParticleData
{
    float Radius;
    std::uint8_t R, G, B;

    ParticleData() = default;
    ParticleData(float radius, std::uint8_t r, std::uint8_t g, std::uint8_t b) : Radius(radius), R(r), G(g), B(b) { }
};