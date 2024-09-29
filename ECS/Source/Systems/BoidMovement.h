#pragma once

//#include "../Math/PartitionGrid.h"
#include "../Math/PartitionGrid2.h"

#include <array>
#include <iostream>
#include <thread>
//#include <mutex>

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

    void Update(float deltaTime)
    {
        auto transformCollection = EcsManager.GetComponentCollection<Transform>();
        auto boidCollection = EcsManager.GetComponentCollection<Boid>();

        //glm::vec2 particleSize = glm::vec2 {1.0f * vision, 1.0f * vision};

        for (const Entity& entity : Entities)
        {
            auto& transform = transformCollection->GetComponent(entity);
            partitionGrid.MoveEntity(entity, transform.Position);
        }


        float noiseRange = (3.1415926f / 80) * noise;
        std::uniform_real_distribution<float> randomNoise(-noiseRange, noiseRange);

        auto alignmentDirections = std::vector(MAXENTITIES, glm::vec2(0.0f, 0.0f));
        auto cohesionDirections = std::vector(MAXENTITIES, glm::vec2(0.0f, 0.0f));
        auto separationDirections = std::vector(MAXENTITIES, glm::vec2(0.0f, 0.0f));
        auto neighbourCounts = std::vector<std::uint32_t>(MAXENTITIES, 0);

        int entityPairs = 0;

        int misses = 0; int treffer = 0;
        for(auto entityPair : partitionGrid.GetEntityPairs())
        {
            auto& transform = transformCollection->GetComponent(entityPair.Entity1);
            auto& boid = boidCollection->GetComponent(entityPair.Entity1);

            auto &otherTransform = transformCollection->GetComponent(entityPair.Entity2);
            auto& otherBoid = boidCollection->GetComponent(entityPair.Entity2);

            float distance = glm::length(transform.Position - otherTransform.Position);

            if (distance > vision){misses++;continue;}treffer++;

            //Alignment
            float b = pow(bias, glm::dot(otherBoid.Velocity, boid.Velocity));
            alignmentDirections[entityPair.Entity1] += otherBoid.Velocity * b;
            alignmentDirections[entityPair.Entity2] += boid.Velocity * b;

            //Cohesion
            cohesionDirections[entityPair.Entity1] += otherTransform.Position;
            cohesionDirections[entityPair.Entity2] += transform.Position;

            //Separation
            float d = distance != 0 ? 1.0f/ (float)distance : 0.00001f;
            separationDirections[entityPair.Entity1] += (transform.Position - otherTransform.Position) * d;
            separationDirections[entityPair.Entity2] += (otherTransform.Position - transform.Position) * d;

            neighbourCounts[entityPair.Entity1]++;
            neighbourCounts[entityPair.Entity2]++;

            entityPairs++;
        }

        //std::cout << "Misses: " << misses << "  Treffer: " << treffer << std::endl;
        //std::cout << entityPairs << std::endl;

        for (const Entity& entity : Entities)
        {
            auto& transform = transformCollection->GetComponent(entity);
            auto& boid = boidCollection->GetComponent(entity);

            boid.Acceleration = glm::vec2{0.0, 0.0};

            //Correct vectors
            if (neighbourCounts[entity] > 0)
            {
                alignmentDirections[entity] = LimitMagnitudeMax(ChangeMagnitude(alignmentDirections[entity], maxSpeed) - boid.Velocity, maxForce);
                cohesionDirections[entity] = LimitMagnitudeMax(ChangeMagnitude((cohesionDirections[entity] / (float) neighbourCounts[entity]++) - transform.Position, maxSpeed) - boid.Velocity, maxForce);
                separationDirections[entity] = LimitMagnitudeMax(ChangeMagnitude(separationDirections[entity], maxSpeed) - boid.Velocity, maxForce);
            }

            //Add vectors to acceleration
            boid.Acceleration += alignmentDirections[entity] * alignment;
            boid.Acceleration += cohesionDirections[entity] * cohesion;
            boid.Acceleration += separationDirections[entity]* separation;

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


            //Debug
            bool debug = false;
            if (debug)
            {
                glLineWidth(1.0f); // Adjust this for a thinner/thicker line

                for (auto bound : partitionGrid.GetCellAreas())
                {
                    // Define the vertices of the rectangle
                    float vertices[] = {
                            bound.Position.x, bound.Position.y,                     // Bottom-left corner
                            bound.Position.x + bound.Size.x, bound.Position.y,       // Bottom-right corner
                            bound.Position.x + bound.Size.x, bound.Position.y + bound.Size.y, // Top-right corner
                            bound.Position.x, bound.Position.y + bound.Size.y        // Top-left corner
                    };

                    // Enable the vertex array and bind the vertices
                    glEnableClientState(GL_VERTEX_ARRAY);
                    glVertexPointer(2, GL_FLOAT, 0, vertices);

                    // Draw the rectangle as a line loop
                    glDrawArrays(GL_LINE_LOOP, 0, 4);

                    // Disable the vertex array
                    glDisableClientState(GL_VERTEX_ARRAY);
                }

                /*for (auto bound : partitionGrid.GetEntityAreas())
                {
                    // Define the vertices of the rectangle
                    float vertices[] = {
                            bound.Position.x, bound.Position.y,                     // Bottom-left corner
                            bound.Position.x + bound.Size.x, bound.Position.y,       // Bottom-right corner
                            bound.Position.x + bound.Size.x, bound.Position.y + bound.Size.y, // Top-right corner
                            bound.Position.x, bound.Position.y + bound.Size.y        // Top-left corner
                    };

                    // Enable the vertex array and bind the vertices
                    glEnableClientState(GL_VERTEX_ARRAY);
                    glVertexPointer(2, GL_FLOAT, 0, vertices);

                    // Draw the rectangle as a line loop
                    glDrawArrays(GL_LINE_LOOP, 0, 4);

                    // Disable the vertex array
                    glDisableClientState(GL_VERTEX_ARRAY);
                }*/
            }
        }

        /*for (const Entity& entity : Entities)
        {
            //Debug - Draw quadtree
            glLineWidth(1.0f); // Adjust this for a thinner/thicker line

            for (auto bound : quadtree.GetQuadBounds())
            {
                // Define the vertices of the rectangle
                float vertices[] = {
                        bound.Position.x, bound.Position.y,                     // Bottom-left corner
                        bound.Position.x + bound.Size.x, bound.Position.y,       // Bottom-right corner
                        bound.Position.x + bound.Size.x, bound.Position.y + bound.Size.y, // Top-right corner
                        bound.Position.x, bound.Position.y + bound.Size.y        // Top-left corner
                };

                // Enable the vertex array and bind the vertices
                glEnableClientState(GL_VERTEX_ARRAY);
                glVertexPointer(2, GL_FLOAT, 0, vertices);

                // Draw the rectangle as a line loop
                glDrawArrays(GL_LINE_LOOP, 0, 4);

                // Disable the vertex array
                glDisableClientState(GL_VERTEX_ARRAY);
            }

            for (auto bound : quadtree.GetObjectBounds())
            {
                // Define the vertices of the rectangle
                float vertices[] = {
                        bound.Position.x, bound.Position.y,                     // Bottom-left corner
                        bound.Position.x + bound.Size.x, bound.Position.y,       // Bottom-right corner
                        bound.Position.x + bound.Size.x, bound.Position.y + bound.Size.y, // Top-right corner
                        bound.Position.x, bound.Position.y + bound.Size.y        // Top-left corner
                };

                // Enable the vertex array and bind the vertices
                glEnableClientState(GL_VERTEX_ARRAY);
                glVertexPointer(2, GL_FLOAT, 0, vertices);

                // Draw the rectangle as a line loop
                glDrawArrays(GL_LINE_LOOP, 0, 4);

                // Disable the vertex array
                glDisableClientState(GL_VERTEX_ARRAY);
            }

        }*/


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
    const float vision = 25.0;
    const float bias = 1.5;
    const float alignment = 1.5f;
    const float cohesion = 1.4;
    const float separation = 1.8;
    const float maxForce = 0.04;
    const float minSpeed = 0.4;
    const float maxSpeed = 0.7;
    const float drag = 0.007;
    const float noise = 2.0;

    std::default_random_engine random;
    PartitionGrid2 partitionGrid = PartitionGrid2();
};