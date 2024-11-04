#pragma once

//#include "../Math/PartitionGrid.h"
#include "../Math/PartitionGrid2.h"

#include <array>
#include <iostream>

class BoidMovement : public System
{
public:
    explicit BoidMovement(ECSWorld* world) : System(world)
    {
        transformCollection = World->GetComponentCollection<Transform>();
        boidCollection = World->GetComponentCollection<Boid>();
    }

    [[nodiscard]] Signature GetSignature() const
    {
        Signature signature;
        signature.set(World->GetComponentType<Transform>());
        signature.set(World->GetComponentType<Boid>());
        return signature;
    }

    void Update(Fixed16_16 deltaTime)
    {
        for (const Entity& entity : Entities)
        {
            auto& transform = transformCollection->GetComponent(entity);
            partitionGrid.MoveEntity(entity, transform.Position);
        }

        Fixed16_16 noiseRange = (Fixed16_16::pi() / Fixed16_16(80)) * noise;
        FixedRandom16_16 randomNoise(-noiseRange, noiseRange);

        auto alignmentDirections = std::vector(MAXENTITIES, Vector2 (0, 0));
        auto cohesionDirections = std::vector(MAXENTITIES, Vector2 (0, 0));
        auto separationDirections = std::vector(MAXENTITIES, Vector2 (0, 0));
        auto neighbourCounts = std::vector<std::int32_t>(MAXENTITIES, 0);

        int entityPairs = 0;

        int misses = 0; int treffer = 0;
        for(auto entityPair : partitionGrid.GetEntityPairs())
        {
            auto& transform = transformCollection->GetComponent(entityPair.Entity1);
            auto& boid = boidCollection->GetComponent(entityPair.Entity1);

            auto &otherTransform = transformCollection->GetComponent(entityPair.Entity2);
            auto& otherBoid = boidCollection->GetComponent(entityPair.Entity2);

            Fixed16_16 distance = (transform.Position - otherTransform.Position).Magnitude();

            if (distance > vision){misses++;continue;}treffer++;

            //Alignment
            Fixed16_16 b = pow(bias, (otherBoid.Velocity.Dot(boid.Velocity)));
            alignmentDirections[entityPair.Entity1] += otherBoid.Velocity * b;
            alignmentDirections[entityPair.Entity2] += boid.Velocity * b;

            //Cohesion
            cohesionDirections[entityPair.Entity1] += otherTransform.Position;
            cohesionDirections[entityPair.Entity2] += transform.Position;

            //Separation
            Fixed16_16 d = distance != Fixed16_16(0) ? Fixed16_16(1) / distance : Fixed16_16(1) / 10000;
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

            boid.Acceleration = Vector2 {0, 0};

            //Correct vectors
            if (neighbourCounts[entity] > 0)
            {
                alignmentDirections[entity] = LimitMagnitudeMax(ChangeMagnitude(alignmentDirections[entity], maxSpeed) - boid.Velocity, maxForce);
                cohesionDirections[entity] = LimitMagnitudeMax(ChangeMagnitude((cohesionDirections[entity] / neighbourCounts[entity]++) - transform.Position, maxSpeed) - boid.Velocity, maxForce);
                separationDirections[entity] = LimitMagnitudeMax(ChangeMagnitude(separationDirections[entity], maxSpeed) - boid.Velocity, maxForce);
            }

            //Add vectors to acceleration
            boid.Acceleration += alignmentDirections[entity] * alignment;
            boid.Acceleration += cohesionDirections[entity] * cohesion;
            boid.Acceleration += separationDirections[entity]* separation;

            boid.Velocity += boid.Acceleration;

            boid.Velocity *= 1 - drag;
            boid.Velocity = Rotate(boid.Velocity, randomNoise(random));

            if (boid.Velocity.Magnitude() == 0)
            {
                //Set velocity to a numberGenerator direction
                boid.Velocity = RandomVector(minSpeed);
            }
            else
            {
                boid.Velocity = LimitMagnitudeMin(boid.Velocity, minSpeed);
            }

            boid.Velocity = LimitMagnitudeMax(boid.Velocity, maxSpeed);
            transform.Position += boid.Velocity * deltaTime * 100;

            //Set to other side of screen when exiting edges
            if (transform.Position.X < 0) transform.Position.X = SCREEN_WIDTH;
            if (transform.Position.X > SCREEN_WIDTH) transform.Position.X = 0;
            if (transform.Position.Y < 0) transform.Position.Y = SCREEN_HEIGHT;
            if (transform.Position.Y > SCREEN_HEIGHT) transform.Position.Y = 0;


            //Debug
            bool debug = false;
            if (debug)
            {
                glLineWidth(1.0f); //Adjust this for a thinner/thicker line

                for (auto bound : partitionGrid.GetCellAreas())
                {
                    //Define the vertices of the rectangle
                    Fixed16_16 vertices[] =
                    {
                        bound.Position.X, bound.Position.Y,                                 // Bottom-left corner
                        bound.Position.X + bound.Size.X, bound.Position.Y,                  // Bottom-right corner
                        bound.Position.X + bound.Size.X, bound.Position.Y + bound.Size.Y,   // Top-right corner
                        bound.Position.X, bound.Position.Y + bound.Size.Y                   // Top-left corner
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

    Vector2 ChangeMagnitude(Vector2 vector, Fixed16_16 magnitude)
    {
        Fixed16_16 length = vector.Magnitude();

        if (length != 0)
        {
            return (vector / length) * magnitude;
        }

        return {0, 0};
    }

    Vector2 LimitMagnitudeMax(Vector2 vector, Fixed16_16 maxMagnitude)
    {
        if (vector.Magnitude() <= maxMagnitude * maxMagnitude)
        {
            return vector;
        }

        return ChangeMagnitude(vector, maxMagnitude);
    }

    Vector2 LimitMagnitudeMin(Vector2 vector, Fixed16_16 minMagnitude)
    {
        if (vector.Magnitude() >= minMagnitude * minMagnitude)
        {
            return vector;
        }

        return ChangeMagnitude(vector, minMagnitude);
    }

    Vector2 Rotate(Vector2 vector, Fixed16_16 angle)
    {
        Fixed16_16 cosine = cos(angle);
        Fixed16_16 sine = sin(angle);

        return {vector.X * cosine - vector.Y * sine, vector.X * sine + vector.Y * cosine};
    }

    Vector2 RandomVector(Fixed16_16 magnitude)
    {
        FixedRandom16_16 randomAngle(Fixed16_16(0), Fixed16_16(1));
        Fixed16_16 angle = randomAngle(random);

        return {fpm::cos(angle) * magnitude, sin(angle) * magnitude};
    }

private:
    const Fixed16_16 vision = Fixed16_16::FromFixed(25, 0);
    const Fixed16_16 bias = Fixed16_16::FromFixed(1, 5);
    const Fixed16_16 alignment = Fixed16_16::FromFixed(1, 5);
    const Fixed16_16 cohesion = Fixed16_16::FromFixed(1, 4);
    const Fixed16_16 separation = Fixed16_16::FromFixed(1, 8);
    const Fixed16_16 maxForce = Fixed16_16::FromFixed(0, 4) / 10;
    const Fixed16_16 minSpeed = Fixed16_16::FromFixed(0, 4);
    const Fixed16_16 maxSpeed = Fixed16_16::FromFixed(0, 7);
    const Fixed16_16 drag = Fixed16_16::FromFixed(0, 7) / 100;
    const Fixed16_16 noise = Fixed16_16::FromFixed(2, 0);

    std::mt19937 random;
    PartitionGrid2 partitionGrid = PartitionGrid2();

    ComponentCollection<Transform>* transformCollection;
    ComponentCollection<Boid>* boidCollection;
};