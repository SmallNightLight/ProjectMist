#pragma once

struct Boid
{
    glm::vec2 Velocity;
    glm::vec2 Acceleration;

    Boid(): Velocity(0.0f, 0.0f), Acceleration(0.0f, 0.0f) { }

    Boid(const glm::vec2& velocity, const glm::vec2& acceleration) : Velocity(velocity), Acceleration(acceleration) { }

};