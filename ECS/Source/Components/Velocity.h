#pragma once

#include <GLM/glm.hpp>

struct Velocity
{
    glm::vec2 Value;

    Velocity() : Value(0.0f, 0.0f) { }
    Velocity(float x, float y) : Value(x, y) { }
    explicit Velocity(const glm::vec2& value) : Value(value) { }
};