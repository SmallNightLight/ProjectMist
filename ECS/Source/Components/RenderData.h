#pragma once

struct RendererData
{
    float X, Y;
    float Width, Height;
    float Red, Green, Blue;

    RendererData()  = default;
    RendererData(float x, float y, float width, float height, float red, float green, float blue)
            : X(x), Y(y), Width(width), Height(height), Red(red), Blue(blue), Green(green) {}
};