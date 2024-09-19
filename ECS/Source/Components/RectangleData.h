#pragma once

#include <cstdint>

struct RectangleData
{
    float Width, Height;
    std::uint8_t R, G, B;

    RectangleData() = default;
    RectangleData(float width, float height, std::uint8_t r, std::uint8_t g, std::uint8_t b) : Width(width), Height(height), R(r), G(g), B(b) { }
};