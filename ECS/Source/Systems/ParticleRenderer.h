#pragma once

#include <GLFW/glfw3.h>
#include <random>

extern ECSManager EcsManager;

class ParticleRenderer : public System
{
public:
    static Signature GetSignature()
    {
        Signature signature;
        signature.set(EcsManager.GetComponentType<Transform>());
        signature.set(EcsManager.GetComponentType<ParticleData>());
        return signature;
    }

    void Render()
    {
        ComponentType transformType = EcsManager.GetComponentType<Transform>();
        ComponentType particleDataType = EcsManager.GetComponentType<ParticleData>();

        for (const Entity& entity : Entities)
        {
            auto& transform = EcsManager.GetComponent<Transform>(entity, transformType);
            auto& particleData = EcsManager.GetComponent<ParticleData>(entity, particleDataType);


            int segments = 3;

            glColor3ub(particleData.R, particleData.G, particleData.B);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(transform.X, transform.Y);

            for (int i = 0; i <= segments; i++)
            {
                float angle = 2.0f * 3.14159f * (float)i / (float)segments;
                float x = transform.X + cos(angle) * particleData.Radius;
                float y = transform.Y + sin(angle) * particleData.Radius;
                glVertex2f(x, y);
            }
            glEnd();
        }
    }
};
