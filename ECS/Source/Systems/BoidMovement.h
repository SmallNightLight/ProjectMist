#pragma once

#include <iostream>

extern ECSManager EcsManager;

class BoidMovement : public System
{
public:
    static Signature GetSignature()
    {
        Signature signature;
        signature.set(EcsManager.GetComponentType<Transform>());
        signature.set(EcsManager.GetComponentType<Boid>());
        return signature;
    }

    void Update(float deltaTime, glm::vec2 mousePosition)
    {
        ComponentType transformType = EcsManager.GetComponentType<Transform>();
        ComponentType boidType = EcsManager.GetComponentType<Boid>();

        float noiseRange = (3.1415926f / 80) * noise;
        std::uniform_real_distribution<float> randomNoise(-noiseRange, noiseRange);

        for (const Entity& entity : Entities)
        {
            auto& transform = EcsManager.GetComponent<Transform>(entity, transformType);
            auto& boid = EcsManager.GetComponent<Boid>(entity, boidType);

            //TODO: Check if this can be local (yes)
            boid.Acceleration = glm::vec2{0.0, 0.0};

            glm::vec2 alignmentDirection = glm::vec2{0.0, 0.0};
            glm::vec2 cohesionDirection = glm::vec2{0.0, 0.0};
            glm::vec2 separationDirection = glm::vec2{0.0, 0.0};

            int neighbourCount = 0;

            for(const Entity& other : Entities)
            {
                auto &otherTransform = EcsManager.GetComponent<Transform>(other, transformType);
                auto& otherBoid = EcsManager.GetComponent<Boid>(other, boidType);

                if (entity == other) continue;

                float distance = glm::length(transform.Position - otherTransform.Position);

                if (distance > vision) continue;

                //Alignment
                float b = pow(bias, glm::dot(otherBoid.Velocity, boid.Velocity));
                alignmentDirection += otherBoid.Velocity * b;

                //Cohesion
                cohesionDirection += otherTransform.Position;

                //Separation
                float d = distance != 0 ? 1.0f/ (float)distance : 0.00001f;
                separationDirection += (transform.Position - otherTransform.Position) * d;

                neighbourCount++;
            }

            //Correct vectors
            if (neighbourCount > 0)
            {
                alignmentDirection = LimitMagnitudeMax(ChangeMagnitude(alignmentDirection, maxSpeed) - boid.Velocity, maxForce);
                cohesionDirection = LimitMagnitudeMax(ChangeMagnitude((cohesionDirection / (float) neighbourCount++) - transform.Position, maxSpeed) - boid.Velocity, maxForce);
                separationDirection = LimitMagnitudeMax(ChangeMagnitude(separationDirection, maxSpeed) - boid.Velocity, maxForce);
            }

            //Add vectors to acceleration
            boid.Acceleration += alignmentDirection * alignment;
            boid.Acceleration += cohesionDirection * cohesion;
            boid.Acceleration += separationDirection * separation;

            boid.Velocity += boid.Acceleration;

            boid.Velocity *= 1 - drag;
            boid.Velocity = Rotate(boid.Velocity, randomNoise(random));

            if (glm::length(boid.Velocity) == 0)
            {
                //Set velocity to a random direction
                boid.Velocity = RandomVector(minSpeed);
            }
            else
            {
                boid.Velocity = LimitMagnitudeMin(boid.Velocity, minSpeed);
            }

            boid.Velocity = LimitMagnitudeMax(boid.Velocity, maxSpeed);
            transform.Position += boid.Velocity * deltaTime * 100.0f;

            //Set to other side of screen when exiting edges
            if (transform.Position.x < 0) transform.Position.x = SCREEN_WIDTH;
            if (transform.Position.x > SCREEN_WIDTH) transform.Position.x = 0;
            if (transform.Position.y < 0) transform.Position.y = SCREEN_HEIGHT;
            if (transform.Position.y> SCREEN_HEIGHT) transform.Position.y = 0;
        }
    }

    glm::vec2 ChangeMagnitude(glm::vec2 vector, float magnitude)
    {
        float length = glm::length(vector);

        if (length != 0.0f)
        {
            return (vector / length) * magnitude;
        }

        return {0.0f, 0.0f};
    }

    glm::vec2 LimitMagnitudeMax(glm::vec2 vector, float maxMagnitude)
    {
        if (glm::length(vector) <= maxMagnitude * maxMagnitude)
        {
            return vector;
        }

        return ChangeMagnitude(vector, maxMagnitude);
    }

    glm::vec2 LimitMagnitudeMin(glm::vec2 vector, float minMagnitude)
    {
        if (glm::length(vector) >= minMagnitude * minMagnitude)
        {
            return vector;
        }

        return ChangeMagnitude(vector, minMagnitude);
    }

    glm::vec2 Rotate(glm::vec2 vector, float angle)
    {
        float cosine = cos(angle);
        float sine = sin(angle);

        return {vector.x * cosine - vector.y * sine, vector.x * sine + vector.y * cosine};
    }

    glm::vec2 RandomVector(float magnitude)
    {
        std::uniform_real_distribution<float> randomAngle(0, 1);
        float angle = randomAngle(random);

        return {cos(angle) * magnitude, sin(angle) * magnitude};
    }

private:
    const float vision = 50.0;
    const float bias = 1.5;
    const float alignment = 1.3f;
    const float cohesion = 1.15;
    const float separation = 1.3;
    const float maxForce = 0.15;
    const float minSpeed = 0.5;
    const float maxSpeed = 2.0;
    const float drag = 0.007;
    const float noise = 1.0;

    std::default_random_engine random;
};