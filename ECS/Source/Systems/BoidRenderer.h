#pragma once

#include "../Rendering/Shader.h"

#include <GLFW/glfw3.h>
#include <random>

class BoidRenderer : public System
{
public:
    explicit BoidRenderer(ECSWorld* world) : System(world)
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

    void Render()
    {
        auto triangleSize = Fixed16_16(6);

        glBegin(GL_TRIANGLES);
        for (const Entity &entity: Entities)
        {
            auto& transform = transformCollection->GetComponent(entity);
            auto& boid = boidCollection->GetComponent(entity);

            Vector2 pos = transform.Position;
            Vector2 vel = boid.Velocity.Normalize();

            Vector2 perpendicular(-vel.Y, vel.X);

            //Define the vertices of the triangle
            Vector2 p1 = pos + vel * triangleSize;                     //Tip of the triangle (facing velocity direction)
            Vector2 p2 = pos - perpendicular * (triangleSize / 2);     //Left base vertex
            Vector2 p3 = pos + perpendicular * (triangleSize / 2);     //Right base vertex

            //Set the color based on speed
            Fixed16_16 speed = boid.Velocity.Magnitude() * 10;
            glColor3f(0, (boid.Velocity.X * Fixed16_16(0, 3) + Fixed16_16(0, 7)).ToFloating<float>(), (boid.Velocity.Y * Fixed16_16(0, 3) + Fixed16_16(0, 7)).ToFloating<float>());

            //Render the triangle
            glVertex2f(p1.X.ToFloating<float>(), p1.Y.ToFloating<float>());
            glVertex2f(p2.X.ToFloating<float>(), p2.Y.ToFloating<float>());
            glVertex2f(p3.X.ToFloating<float>(), p3.Y.ToFloating<float>());
        }
        glEnd();
    }

    ComponentCollection<Transform>* transformCollection;
    ComponentCollection<Boid>* boidCollection;
};