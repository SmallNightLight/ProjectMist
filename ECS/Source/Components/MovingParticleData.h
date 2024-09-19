#pragma once

#include <cstdint>

struct MovingParticleData
{
    float X, Y;
    float VelocityX, VelocityY;

    float Radius;
    std::uint8_t R, G, B;

    MovingParticleData() = default;
    MovingParticleData(float x, float y, float velocityX, float velocityY, float radius, std::uint8_t r, std::uint8_t g, std::uint8_t b) : X(x), Y(y), VelocityX(velocityX), VelocityY(velocityY), Radius(radius), R(r), G(g), B(b) { }
};