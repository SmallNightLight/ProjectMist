#pragma once

#include "../Rendering/Shader.h"

#include <GLFW/glfw3.h>
#include <random>

extern ECSManager EcsManager;

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
        ComponentType transformType = EcsManager.GetComponentType<Transform>();
        ComponentType boidType = EcsManager.GetComponentType<Boid>();

        float triangleSize = 10;

        glBegin(GL_TRIANGLES);
        for (const Entity &entity: Entities)
        {
            auto &transform = EcsManager.GetComponent<Transform>(entity, transformType);
            auto &boid = EcsManager.GetComponent<Boid>(entity, boidType);

            glm::vec2 pos = transform.Position;
            glm::vec2 vel = glm::normalize(boid.Velocity);

            glm::vec2 perpendicular(-vel.y, vel.x);

            //Define the vertices of the triangle
            glm::vec2 p1 = pos + vel * triangleSize;                        //Tip of the triangle (facing velocity direction)
            glm::vec2 p2 = pos - perpendicular * (triangleSize / 2.0f);     //Left base vertex
            glm::vec2 p3 = pos + perpendicular * (triangleSize / 2.0f);     //Right base vertex

            //Set the color based on speed
            float speed = glm::length(boid.Velocity) / 2.0f;
            glColor3f(0, 1, speed);

            //Render the triangle
            glVertex2f(p1.x, p1.y);
            glVertex2f(p2.x, p2.y);
            glVertex2f(p3.x, p3.y);
        }
        glEnd();
    }
};