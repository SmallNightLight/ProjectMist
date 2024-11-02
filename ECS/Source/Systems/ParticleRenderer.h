#pragma once

#include "../Rendering/Shader.h"

#include <GLFW/glfw3.h>
#include <random>

class ParticleRenderer : public System
{
public:
    explicit ParticleRenderer(ECSWorld* world) : System(world), shader()
    {
        transformCollection = World->GetComponentCollection<Transform>();
        velocityCollection = World->GetComponentCollection<Velocity>();

        shader.InitializeFromSource(vertexSource, fragmentSource);
    }

    [[nodiscard]] Signature GetSignature() const
    {
        Signature signature;
        signature.set(World->GetComponentType<Transform>());
        signature.set(World->GetComponentType<Velocity>());
        return signature;
    }

    void Render()
    {
        glPointSize(4);
        glEnable(GL_POINT_SMOOTH);
        glBegin(GL_POINTS);
        for (const Entity& entity : Entities)
        {
            auto& transform = transformCollection->GetComponent(entity);
            auto& velocity = velocityCollection->GetComponent(entity);

            //glColor3ub(fpm::ceilInt(velocity.Value.X), 0, fpm::ceilInt(velocity.Value.Y));
            glColor3ub(255, 0, 0);
            glVertex2f(transform.Position.X.ToFloating<float>(), transform.Position.Y.ToFloating<float>());
        }
        glEnd();
    }

private:
    ComponentCollection<Transform>* transformCollection;
    ComponentCollection<Velocity>* velocityCollection;

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