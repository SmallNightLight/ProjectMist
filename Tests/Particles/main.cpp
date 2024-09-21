#include "Shader.h"

#include <GLM/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <array>

#define SCREEN_WIDTH 1680
#define SCREEN_HEIGHT 1050

GLuint vao, vbo;
Shader shader;

glm::vec2 gravityCenter {0, 0};
float damping = 0.997f;
float attractorMass = 100.0f;
float particleMass = 10.0f;
float gravity = 0.4f;
float softening = 50.0f;

const int MAXPARTICLES = 100000;
const int ParticleAttributeCount = 4;

std::array<glm::vec2, MAXPARTICLES> positions;
std::array<glm::vec2, MAXPARTICLES> velocities;

void InitializeParticles()
{
    for (int i = 0; i < MAXPARTICLES; ++i)
    {
        positions[i] = { static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f, static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f }; //Random [-1, 1]
    }
}

void SetFrameSize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void InitOpenGLBuffers()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES * ParticleAttributeCount * sizeof(float), nullptr, GL_STATIC_DRAW);

    //Set up instanced attributes
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(MAXPARTICLES * sizeof(glm::vec2))); // Velocity
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);
}

void RenderParticles()
{
    shader.Use();

    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_POINTS, 0, 1, MAXPARTICLES);
    glBindVertexArray(0);
}

double GetDeltaTime(double& lastTime)
{
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    return deltaTime;
}

void UpdateGravityCenterBasedOnMouse(GLFWwindow* window)
{
    //Get the mouse position in pixel coordinates
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    //Convert mouse X and Y to normalized screen coordinates [-1, 1]
    gravityCenter.x = ((float)mouseX / SCREEN_WIDTH) * 2.0f - 1.0f;
    gravityCenter.y = 1.0f - ((float)mouseY / SCREEN_HEIGHT) * 2.0f;
}

void UpdateParticles(float deltaTime)
{
    float combinedMass = gravity * attractorMass * particleMass;

    for (int i = 0; i < MAXPARTICLES; ++i)
    {
        glm::vec2 r = gravityCenter - positions[i];
        float rSquared = glm::length(r) + softening;
        glm::vec2 force = (combinedMass * glm::normalize(r) / rSquared);

        glm::vec2 acceleration = force / particleMass;
        positions[i] = positions[i] + (velocities[i] * deltaTime + 0.5f * acceleration * deltaTime * deltaTime);
        velocities[i] = velocities[i] + acceleration * deltaTime;
    }

    //Update buffer data with the new data
    glBufferSubData(GL_ARRAY_BUFFER, 0, MAXPARTICLES * sizeof(glm::vec2), positions.data());
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * MAXPARTICLES, MAXPARTICLES * sizeof(glm::vec2), velocities.data());
}

int main()
{
    if (!glfwInit())
    {
        return -1;
    }

    //Create adn initialize a new window
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Particle System - Instancing", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    //Disable vsync (0 = Disabled)
    glfwSwapInterval( 0 );

    //Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return -1;
    }

    //Set the viewport
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glfwSetFramebufferSizeCallback(window, SetFrameSize);

    //Initialize shaders and particles
    shader.InitializeFromPath("Shaders/vertex.glsl", "Shaders/fragment.glsl");
    InitializeParticles();
    InitOpenGLBuffers();

    double lastTime = 0.0;
    double lastTitleUpdateTime = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        UpdateGravityCenterBasedOnMouse(window);

        //Calculate delta time
        double deltaTime = GetDeltaTime(lastTime);

        //Render particles
        UpdateParticles((float)deltaTime);
        RenderParticles();

        //Update the framerate every second
        double currentTime = glfwGetTime();
        if (currentTime - lastTitleUpdateTime >= 1.0f)
        {
            std::ostringstream title;
            title << "Particle System - FPS: " << static_cast<int>(1.0 / deltaTime);
            glfwSetWindowTitle(window, title.str().c_str());
            lastTitleUpdateTime = currentTime;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //Cleanup buffers and stop glfw
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}