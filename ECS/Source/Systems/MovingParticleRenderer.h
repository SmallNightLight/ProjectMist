#pragma once

#include <GLFW/glfw3.h>
#include <random>

extern ECSManager EcsManager;

class MovingParticleRenderer : public System
{
public:
    static Signature GetSignature()
    {
        Signature signature;
        signature.set(EcsManager.GetComponentType<MovingParticleData>());
        return signature;
    }

    void Render()
    {
        ComponentType movingParticleDataType = EcsManager.GetComponentType<MovingParticleData>();

        for (const Entity& entity : Entities)
        {
            auto& movingParticleData = EcsManager.GetComponent<MovingParticleData>(entity, movingParticleDataType);

            //Movement
            movingParticleData.VelocityX = 0;
            movingParticleData.VelocityY = -0.02f;

            movingParticleData.X += movingParticleData.VelocityX;
            movingParticleData.Y += movingParticleData.VelocityY;

            if (movingParticleData.Y < 0)
            {
                movingParticleData.Y = 700;
            }

            //Rendering
            int segments = 10;

            glColor3ub(movingParticleData.R, movingParticleData.G, movingParticleData.B);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(movingParticleData.X, movingParticleData.Y);

            for (int i = 0; i <= segments; i++)
            {
                float angle = 2.0f * 3.14159f * (float)i / (float)segments;
                float x = movingParticleData.X + cos(angle) * movingParticleData.Radius;
                float y = movingParticleData.Y + sin(angle) * movingParticleData.Radius;
                glVertex2f(x, y);
            }
            glEnd();
        }
    }
};
