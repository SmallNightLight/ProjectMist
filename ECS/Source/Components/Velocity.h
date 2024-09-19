#pragma once

struct Velocity
{
    float X, Y;

    Velocity() = default;
    Velocity(float x, float y) : X(x), Y(y) { }
};