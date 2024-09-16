#pragma once

#include <SFML/Graphics.hpp>
#include <GLFW/glfw3.h>
#include <random>

extern ECSManager EcsManager;

class Renderer : public System
{
public:
    static Signature GetSignature()
    {
        Signature signature;
        signature.set(EcsManager.GetComponentType<RendererData>());
        return signature;
    }

    void Render()
    {
        for (const Entity& entity : Entities)
        {
            auto& renderData = EcsManager.GetComponent<RendererData>(entity);

            //Draw red rectangle
            glColor3f(renderData.Red, renderData.Green, renderData.Blue);
            glBegin(GL_QUADS);

            glVertex2f(renderData.X, renderData.Y);
            glVertex2f(renderData.X + renderData.Width, renderData.Y);
            glVertex2f(renderData.X + renderData.Width, renderData.Y - renderData.Height);
            glVertex2f(renderData.X, renderData.Y - renderData.Height);

            glEnd();
        }
    }
};