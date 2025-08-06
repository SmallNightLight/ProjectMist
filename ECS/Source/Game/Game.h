#pragma once

#include "../ECS/ECS.h"

#include "../Components/ComponentHeaders.h"
#include "../Systems/SystemHeader.h"
#include "../Physics/Physics.h"

#include "GameSettings.h"
#include "WorldManager.h"
#include "Worlds/PhysicsWorld.h"

#include "Input/InputData.h"
#include "Input/InputManager.h"
#include "Input/Input.h"

#include <iostream>
#include <sstream>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Networking/Client/ClientHandler.h"
#include "Input/InputCollection.h"

class Game
{
public:
    explicit Game() : worldManager(WorldManager()), playerInput(Input(playerInputKeys)), otherInput(Input(otherInputKeys)), clientHandler(ClientHandler("localhost", "50000"))
    {
        if (InitializeOpenGL() != 0) return;

        InputManager::SetupCallbacks(window);

        AddWorlds();
    }

    int InitializeOpenGL()
    {
        //Initialize OpenGL
        if (!glfwInit())
        {
            return -1;
        }

        //Create and initialize a new window
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
        Fixed16_16 fixedDelta = Fixed16_16(1) / Fixed16_16(SimulationFPS);
        double lastTime = glfwGetTime();
        Fixed16_16 accumulator = Fixed16_16(0);
        double lastTitleUpdateTime = 0.0;

        //Time tracking variables
        double updateRenderTime = 0.0;
        double sleepTime = 0.0;
        double averageFrameTime = 0.0;

        uint32_t frameCount = 0;

        //Start client thread
        clientHandler.Start();

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

            if (isPaused && otherInput.GetKeyDown(GLFW_KEY_RIGHT))
            {
                Update(window, fixedDelta);
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

            if (otherInput.GetKey(GLFW_KEY_LEFT_CONTROL) && otherInput.GetKeyDown(GLFW_KEY_C))
            {
                //Serialize current game state
                std::cout << "Serializing Game State" << std::endl;
                Serialize();
            }

            if (hasSerializedStream && otherInput.GetKey(GLFW_KEY_LEFT_CONTROL) && otherInput.GetKeyDown(GLFW_KEY_V))
            {
                //Deserialize the saved game state amd override the current game state
                std::cout << "Deserializing Game State" << std::endl;
                Deserialize();
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
            Render();

            //Handle input
            playerInput.Update();
            otherInput.Update();
            glfwPollEvents();

            //Time spent on updates and rendering
            double frameUpdateRenderEnd = glfwGetTime();
            updateRenderTime += frameUpdateRenderEnd - frameUpdateRenderStart;

            //Sleep to maintain frame pacing
            double frameEndTime = glfwGetTime();
            double targetFrameTime = fixedDelta.ToFloating<float>();
            double elapsedFrameTime = frameEndTime - frameStartTime;

            ++frameCount;
            averageFrameTime += elapsedFrameTime;

            if (elapsedFrameTime < targetFrameTime)
            {
                double sleepDuration = targetFrameTime - elapsedFrameTime;
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepDuration));
                sleepTime += sleepDuration;
            }

            auto oldPhysicsWorld = worldManager.GetWorld<PhysicsWorld>(physicsWorldType);
            if (oldPhysicsWorld->GetCurrentFrame() >= 1000) break;
        }

        std::cout << "Average frame time (Lower is better) ms: " << 1000 * averageFrameTime / frameCount << std::endl;

        //Stop glfw
        glfwDestroyWindow(window);
        glfwTerminate();

        return 0;
    }

    void AddWorlds()
    {
        physicsWorldType = worldManager.AddWorld<PhysicsWorld>();

        //Add objects to current world
        worldManager.GetWorld<PhysicsWorld>(physicsWorldType)->AddObjects();

        //Setup cache on all layers
        for (auto physicsWorld : worldManager.GetAllWorlds<PhysicsWorld>(physicsWorldType))
        {
            physicsWorld->InitializeCache(&cacheManager);
        }
    }

    void Update(GLFWwindow* window, Fixed16_16 deltaTime)
    {
        auto oldPhysicsWorld = worldManager.GetWorld<PhysicsWorld>(physicsWorldType);
        FrameNumber currentFrame = oldPhysicsWorld->GetCurrentFrame();

        //Update the input of this client
        InputData input = playerInput.GetInputData(currentFrame);
        clientHandler.SendInput(input);
        clientHandler.UpdateInput(clientHandler.GetClientID(), input);

        //Update the input of other clients and handle other packets
        clientHandler.ReadMessages(oldPhysicsWorld);

        FrameNumber rollbackFrames = clientHandler.GetRollbacks(oldPhysicsWorld->GetCurrentFrame());
        FrameNumber lastConfirmedFrame = clientHandler.GetLastConfirmedFrame();

        if (RollbackDebugMode)
            rollbackFrames = MaxRollBackFrames - 1;

        //TODO for optimization: Delta rollback, not save every physics tick, threaded physics, grid physics broad phase

        if (rollbackFrames > 0)
        {
            FrameNumber actualRollbacks = 0;

            if (worldManager.Rollback(rollbackFrames))
            {
                actualRollbacks = rollbackFrames;
                //std::cout << "Rollback " << rollbackFrames<< " frames" << std::endl;
            }
            else
            {
                std::cout << "Could not rollback this many frames:  " << rollbackFrames << std::endl;
            }

            for(int i = 0; i < actualRollbacks; ++i)
            {
                worldManager.NextFrame<PhysicsWorld>(physicsWorldType, currentFrame - actualRollbacks + i == lastConfirmedFrame);

                auto p2 = worldManager.GetWorld<PhysicsWorld>(physicsWorldType);
                std::vector<Input*> inputs = clientHandler.GetAllClientInputs(p2->GetCurrentFrame());
                p2->Update(deltaTime, inputs, clientHandler.GetClientID());
            }
        }

        worldManager.NextFrame<PhysicsWorld>(physicsWorldType, currentFrame == lastConfirmedFrame);
        auto physicsWorld = worldManager.GetWorld<PhysicsWorld>(physicsWorldType);

        if (physicsWorld->GetCurrentFrame() != currentFrame)
        {
            //Received new game data from another client
            worldManager.PreventFurtherRollback();

            std::vector<Input*> inputs = clientHandler.GetAllClientInputs(currentFrame);
            physicsWorld->Update(deltaTime, inputs, clientHandler.GetClientID());
        }
        else
        {
            std::vector<Input*> inputs = clientHandler.GetAllClientInputs(currentFrame);
            physicsWorld->Update(deltaTime, inputs, clientHandler.GetClientID());

            clientHandler.SendGameData(physicsWorld);
        }
    }

    void Render()
    {
        glClear(GL_COLOR_BUFFER_BIT);

        auto physicsWorld = worldManager.GetWorld<PhysicsWorld>(physicsWorldType);
        physicsWorld->Render();

        glfwSwapBuffers(window);
    }

    void Serialize()
    {
        auto physicsWorld = worldManager.GetWorld<PhysicsWorld>(physicsWorldType);
        serializedStream.Clear();
        hasSerializedStream = true;
        physicsWorld->Serialize(serializedStream);
    }

    void Deserialize()
    {
        auto physicsWorld = worldManager.GetWorld<PhysicsWorld>(physicsWorldType);
        Stream temporaryStream = Stream(serializedStream);
        physicsWorld->Deserialize(temporaryStream);
        worldManager.PreventFurtherRollback();
    }

private:
    static void SetFrameSize(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

private:
    WorldManager worldManager;
    WorldType physicsWorldType;

    CacheManager cacheManager;

    GLFWwindow* window;

    Input playerInput;
    Input otherInput;

    ClientHandler clientHandler;

    //Debug
    bool hasSerializedStream;
    Stream serializedStream;

    bool isPaused;
};