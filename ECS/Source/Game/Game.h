#pragma once

#include "../ECS/ECS.h"

#include "../Components/ComponentHeaders.h"
#include "../Systems/SystemHeader.h"
#include "../Physics/Physics.h"

#include "GameSettings.h"
#include "WorldManager.h"
#include "Worlds/PhysicsWorld.h"
#include "Input/InputManager.h"
#include "Input/Input.h"

#include <iostream>
#include <sstream>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Game
{
public:
    explicit Game() : worldManager(WorldManager()), playerInput(Input(playerInputKeys)), otherInput(Input(otherInputKeys))
    {
        if (InitializeOpenGL() != 0) return;

        InputManager::SetupCallbacks(window);

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
        isPaused = false;
        Fixed16_16 fixedDelta = Fixed16_16(1) / Fixed16_16(60);
        double lastTime = glfwGetTime();
        Fixed16_16 accumulator = Fixed16_16(0);
        double lastTitleUpdateTime = 0.0;

        //Time tracking variables
        double updateRenderTime = 0.0;
        double sleepTime = 0.0;

        while (!glfwWindowShouldClose(window))
        {
            //Calculate delta time
            double currentTime = glfwGetTime();
            double frameTime = currentTime - lastTime;
            lastTime = currentTime;

            //Limit the frame time to avoid spiral of death (large lag spikes)
            frameTime = std::min(frameTime, 0.25);
            accumulator += Fixed16_16::FromFloat(frameTime);

            //Reset time tracking variables for this frame
            double frameStartTime = glfwGetTime();
            double frameUpdateRenderStart = frameStartTime;

            if (currentTime - lastTitleUpdateTime >= 1.0f)
            {
                double totalFrameTime = updateRenderTime + sleepTime;
                double updateRenderPercent = (updateRenderTime / totalFrameTime) * 100.0;
                double sleepPercent = (sleepTime / totalFrameTime) * 100.0;

                std::ostringstream title;
                title << "ECS Test - FPS: " << static_cast<int>(1.0 / frameTime)
                      << " | Update/Render: " << static_cast<int>(updateRenderPercent) << "%"
                      << " | Waiting: " << static_cast<int>(sleepPercent) << "%";
                glfwSetWindowTitle(window, title.str().c_str());

                // Reset tracking for the next second
                updateRenderTime = 0.0;
                sleepTime = 0.0;
                lastTitleUpdateTime = currentTime;
            }

            if (otherInput.GetKeyDown(GLFW_KEY_SPACE))
            {
                isPaused = !isPaused;
            }

            if (otherInput.GetKeyDown(GLFW_KEY_ESCAPE))
            {
                glfwSetWindowShouldClose(window, true);
                break;
            }

            if (otherInput.GetKeyDown(GLFW_KEY_R))
            {
                isPaused = true;
                worldManager.Rollback(1);
            }

            while (accumulator >= fixedDelta)
            {
                if (!isPaused)
                {
                    Update(window, fixedDelta);
                }
                accumulator -= fixedDelta;
            }

            //Render frame
            glClear(GL_COLOR_BUFFER_BIT);
            Render();
            glfwSwapBuffers(window);

            //Handle input
            playerInput.Update();
            otherInput.Update();
            glfwPollEvents();

            // Time spent on updates and rendering
            double frameUpdateRenderEnd = glfwGetTime();
            updateRenderTime += frameUpdateRenderEnd - frameUpdateRenderStart;

            //Sleep to maintain frame pacing
            double frameEndTime = glfwGetTime();
            double targetFrameTime = fixedDelta.ToFloating<float>();
            double elapsedFrameTime = frameEndTime - frameStartTime;

            if (elapsedFrameTime < targetFrameTime)
            {
                double sleepDuration = targetFrameTime - elapsedFrameTime;
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepDuration));
                sleepTime += sleepDuration;
            }
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
        physicsWorld->Update(deltaTime, playerInput, numberGenerator);
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
    Input<3> playerInput;
    Input<3> otherInput;

    std::mt19937 numberGenerator;

    bool isPaused;
};