#pragma once

struct Transform
{
    float X, Y;

    Transform() = default;
    Transform(float x, float y) : X(x), Y(y) { }
};