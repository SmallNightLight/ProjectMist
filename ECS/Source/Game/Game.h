#pragma once

#include "../ECS/ECS.h"

#include "../Components/ComponentHeaders.h"
#include "../Systems/SystemHeader.h"
#include "../Physics/Physics.h"

#include "GameSettings.h"
#include "WorldManager.h"
#include "Worlds/PhysicsWorld.h"
#include "Input/InputManager.h"

#include <iostream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Game
{
public:
    explicit Game() : worldManager(WorldManager()), input(InputManager(inputKeys))
    {
        if (InitializeOpenGL() != 0) return;

        InputManager<3>::SetupCallbacks(window);
        numberGenerator = std::mt19937(12);

        AddWorlds();
    }

    int InitializeOpenGL()
    {
        //Initialize OpenGL
        if (!glfwInit())
        {
            return -1;
        }

        //Create adn initialize a new window
        window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Particle System - Instancing", nullptr, nullptr);
        if (!window)
        {
            glfwTerminate();
            return -1;
        }

        glfwMakeContextCurrent(window);

        //Disable vsync (0 = Disabled)
        glfwSwapInterval( 0 );

        //Initialize GLAD
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        {
            return -1;
        }

        glfwSetFramebufferSizeCallback(window, SetFrameSize);

        return 0;
    }

    int GameLoop()
    {
        double lastTime = 0.0;
        double lastTitleUpdateTime = 0.0;

        while (!glfwWindowShouldClose(window))
        {
            //Calculate delta time
            double currentTime = glfwGetTime();
            auto deltaTime = static_cast<float>(currentTime - lastTime);
            lastTime = currentTime;

            if (currentTime - lastTitleUpdateTime >= 1.0f)
            {
                std::ostringstream title;
                title << "ECS Test - FPS: " << static_cast<int>(1.0f / deltaTime);
                glfwSetWindowTitle(window, title.str().c_str());
                lastTitleUpdateTime = currentTime;
            }

            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                isPaused = !isPaused;
                glfwWaitEventsTimeout(0.3);
            }

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(window, true);
                break;
            }

            //Clear frame
            glClear(GL_COLOR_BUFFER_BIT);

            Fixed16_16 fixedDelta = Fixed16_16::FromFloat(deltaTime);
            Fixed16_16 setDelta = Fixed16_16(1) / Fixed16_16(600);
            if (!isPaused)
            {
                Update(window, fixedDelta);
            }

            Render();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        //Stop glfw
        glfwDestroyWindow(window);
        glfwTerminate();

        return 0;
    }

    void AddWorlds()
    {
        physicsWorldType = worldManager.AddWorld<PhysicsWorld>();
        worldManager.GetWorld<PhysicsWorld>(physicsWorldType)->AddObjects(numberGenerator);
    }

    void Update(GLFWwindow* window, Fixed16_16 deltaTime)
    {
        worldManager.NextFrame();

        auto physicsWorld = worldManager.GetWorld<PhysicsWorld>(physicsWorldType);
        physicsWorld->Update(deltaTime, input, numberGenerator);

        input.Update();
    }

    void Render()
    {
        auto physicsWorld = worldManager.GetWorld<PhysicsWorld>(physicsWorldType);
        physicsWorld->Render();
    }

private:
    static void SetFrameSize(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }


    WorldManager worldManager;
    WorldType physicsWorldType;

    GLFWwindow* window;
    InputManager<3> input;
    static constexpr std::array<u_int16_t, 3> inputKeys = {GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT, GLFW_MOUSE_BUTTON_MIDDLE};

    std::mt19937 numberGenerator;

    bool isPaused;
};