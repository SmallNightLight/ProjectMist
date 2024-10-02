#pragma once

#include "../Rendering/Shader.h"

#include <GLFW/glfw3.h>
#include <random>

class BoidRenderer : public System
{
public:
    static Signature GetSignature()
    {
        Signature signature;
        signature.set(EcsManager.GetComponentType<Transform>());
        signature.set(EcsManager.GetComponentType<Boid>());
        return signature;
    }

    void Render()
    {
        auto transformCollection = EcsManager.GetComponentCollection<Transform>();
        auto boidCollection = EcsManager.GetComponentCollection<Boid>();

        float triangleSize = 6;

        glBegin(GL_TRIANGLES);
        for (const Entity &entity: Entities)
        {
            auto& transform = transformCollection->GetComponent(entity);
            auto& boid = boidCollection->GetComponent(entity);

            glm::vec2 pos = transform.Position;
            glm::vec2 vel = glm::normalize(boid.Velocity);

            glm::vec2 perpendicular(-vel.y, vel.x);

            //Define the vertices of the triangle
            glm::vec2 p1 = pos + vel * triangleSize;                        //Tip of the triangle (facing velocity direction)
            glm::vec2 p2 = pos - perpendicular * (triangleSize / 2.0f);     //Left base vertex
            glm::vec2 p3 = pos + perpendicular * (triangleSize / 2.0f);     //Right base vertex

            //Set the color based on speed
            float speed = glm::length(boid.Velocity) * 10.0f - 6.0f * sin(entity);
            glColor3f(0, boid.Velocity.x * 0.3f + 0.7f, boid.Velocity.y * 0.3f + 0.7f);// * 0.5f + 0.5f);

            //Render the triangle
            glVertex2f(p1.x, p1.y);
            glVertex2f(p2.x, p2.y);
            glVertex2f(p3.x, p3.y);
        }
        glEnd();
    }
};