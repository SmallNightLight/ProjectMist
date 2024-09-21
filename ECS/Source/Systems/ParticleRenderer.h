#pragma once

#include "../Rendering/Shader.h"

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
        signature.set(EcsManager.GetComponentType<Velocity>());
        return signature;
    }

    ParticleRenderer() : shader()
    {
       shader.InitializeFromSource(vertexSource, fragmentSource);
    }

    void Render()
    {
        ComponentType transformType = EcsManager.GetComponentType<Transform>();
        ComponentType velocityType = EcsManager.GetComponentType<Velocity>();

        glPointSize(4);
        glEnable(GL_POINT_SMOOTH);
        glBegin(GL_POINTS);
        for (const Entity& entity : Entities)
        {
            auto& transform = EcsManager.GetComponent<Transform>(entity, transformType);
            auto& velocity = EcsManager.GetComponent<Velocity>(entity, velocityType);

            glColor3ub(velocity.Value.x, 0, velocity.Value.y);
            glVertex2f(transform.Position.x, transform.Position.y);
        }
        glEnd();
    }

private:
    Shader shader;

    const char* vertexSource = "#version 330 core\n"
                               "layout(location = 1) in vec2 position;\n"
                               "layout(location = 2) in vec2 velocity;\n"
                               "\n"
                               "out vec3 fragColor;\n"
                               "\n"
                               "vec3 hsv2rgb(vec3 c)\n"
                               "{\n"
                               "    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n"
                               "    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\n"
                               "    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\n"
                               "}\n"
                               "\n"
                               "void main()\n"
                               "{\n"
                               "    gl_Position = vec4(position, 0.0, 1.0);\n"
                               "\n"
                               "    float speed = length(velocity) * 0.3f;\n"
                               "    fragColor = hsv2rgb(vec3(speed, 1, 1));\n"
                               "}";

    const char* fragmentSource = "#version 330 core\n"
                                 "in vec3 fragColor;\n"
                                 "out vec4 color;\n"
                                 "\n"
                                 "void main()\n"
                                 "{\n"
                                 "    color = vec4(fragColor, 1.0);\n"
                                 "}";
};