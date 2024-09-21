#pragma once

#include <GLFW/glfw3.h>
#include <random>

extern ECSManager EcsManager;

class ParticleRenderer : public System
{
    GLuint VAO, VBO[2];

public:
    static Signature GetSignature()
    {
        Signature signature;
        signature.set(EcsManager.GetComponentType<Transform>());
        signature.set(EcsManager.GetComponentType<ParticleData>());
        return signature;
    }

    void Setup()
    {

    }

    void Render()
    {
        ComponentType transformType = EcsManager.GetComponentType<Transform>();
        ComponentType particleDataType = EcsManager.GetComponentType<ParticleData>();

        glPointSize(20);
        glEnable(GL_POINT_SMOOTH);
        glBegin(GL_POINTS);
        for (const Entity& entity : Entities)
        {
            auto& transform = EcsManager.GetComponent<Transform>(entity, transformType);
            auto& particleData = EcsManager.GetComponent<ParticleData>(entity, particleDataType);

            glColor3ub(particleData.R, particleData.G, particleData.B);
            glVertex2f(transform.X, transform.Y);
        }
        glEnd();
    }
};
