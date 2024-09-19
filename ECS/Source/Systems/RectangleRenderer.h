#pragma once

#include <GLFW/glfw3.h>
#include <random>

extern ECSManager EcsManager;

class RectangleRenderer : public System
{
public:
    static Signature GetSignature()
    {
        Signature signature;
        signature.set(EcsManager.GetComponentType<Transform>());
        signature.set(EcsManager.GetComponentType<RectangleData>());
        return signature;
    }

    void Render()
    {
        ComponentType transformType = EcsManager.GetComponentType<Transform>();
        ComponentType rectangleDataType = EcsManager.GetComponentType<RectangleData>();

        for (const Entity& entity : Entities)
        {
            auto& transform = EcsManager.GetComponent<Transform>(entity, transformType);
            auto& rectangleData = EcsManager.GetComponent<RectangleData>(entity, rectangleDataType);

            glColor3ub(rectangleData.R, rectangleData.G, rectangleData.B);
            glBegin(GL_QUADS);

            glVertex2f(transform.X, transform.Y);
            glVertex2f(transform.X + rectangleData.Width, transform.Y);
            glVertex2f(transform.X + rectangleData.Width, transform.Y - rectangleData.Height);
            glVertex2f(transform.X, transform.Y - rectangleData.Height);

            glEnd();
        }
    }
};