#pragma once

#include <GLM/glm.hpp>

struct Velocity
{
    glm::vec2 Value;
    glm::vec2 Acceleration;

    Velocity() : Value(0.0f, 0.0f), Acceleration(glm::vec2{0, 0}) { }
    Velocity(float x, float y) : Value(x, y), Acceleration(glm::vec2{0, 0}) { }
    explicit Velocity(const glm::vec2& value) : Value(value), Acceleration(glm::vec2{0, 0}) { }
};