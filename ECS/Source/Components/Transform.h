#pragma once

#include <glm.h>

struct Transform
{
    glm::vec2 Position;

    Transform() : Position(0.0f, 0.0f) { }
    Transform(float x, float y) : Position(x, y) { }
    explicit Transform(const glm::vec2& value) : Position(value) { }
};