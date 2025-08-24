#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Physics/Physics.h"
#include "../Networking/Client/ClientHandler.h"

#include "GameSettings.h"
#include "WorldManager.h"
#include "CacheManager.h"
#include "Worlds/PhysicsWorld.h"

#include "Input/InputData.h"
#include "Input/InputManager.h"
#include "Input/Input.h"

#include <iostream>
#include <sstream>
#include <thread>

class Game
{
public:
    explicit Game() : worldManager(WorldManager()), playerInput(Input(playerInputKeys)), otherInput(Input(otherInputKeys)), clientHandler(ClientHandler("localhost", "50000"))
    {
        if (InitializeOpenGL() != 0) return;

        InputManager::SetupCallbacks(window);

        AddObjects();
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
                worldManager.Restore();
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

            if (worldManager.GetPhysicsWorld().GetCurrentFrame() >= 1000) break;
        }

        std::cout << "Average frame time (Lower is better) ms: " << 1000 * averageFrameTime / frameCount << std::endl;

        //Stop glfw
        glfwDestroyWindow(window);
        glfwTerminate();

        return 0;
    }

    void AddObjects()
    {
        //Add objects
        worldManager.GetPhysicsWorld().AddObjects();

        //Setup cache
        worldManager.GetPhysicsWorld().InitializeCache(&cacheManager);
    }

    void Update(GLFWwindow* window, Fixed16_16 deltaTime)
    {
        glClear(GL_COLOR_BUFFER_BIT); ///////TODO impORTANTY back to render
        PhysicsWorld& basePhysicsWorld = worldManager.GetPhysicsWorld();
        FrameNumber currentFrame = basePhysicsWorld.GetCurrentFrame();

        //Update the input of this client
        InputData input = playerInput.GetInputData(currentFrame);
        clientHandler.SendInput(input);
        clientHandler.UpdateInput(clientHandler.GetClientID(), input);

        //Update the input of other clients and handle other packets, deserializing new game state
        bool newGameData = false;
        clientHandler.ReadMessages(basePhysicsWorld, newGameData);

        FrameNumber lastConfirmedFrame = clientHandler.GetLastConfirmedFrame();

        if (RollbackDebugMode)
        {
            if (currentFrame < MaxRollBackFrames)
                lastConfirmedFrame = 1;
            else
                lastConfirmedFrame = currentFrame - (MaxRollBackFrames - 1);
        }

        if (lastConfirmedFrame > currentFrame)
        {
            //Should only be possible at start
            lastConfirmedFrame = currentFrame;
        }

        if (newGameData)
        {
            worldManager.Reset(); //TODO: With delay frames might still be behind
            lastConfirmedFrame = currentFrame;
        }
        else if (lastConfirmedFrame < currentFrame)
        {
            if (currentFrame - lastConfirmedFrame >= MaxRollBackFrames)
            {
                std::cout << "Could not rollback " << currentFrame - lastConfirmedFrame << "frames" << std::endl;
            }
            else
            {
                int32_t rollbackCount = worldManager.Restore();

                if (rollbackCount < 0)
                {
                    std::cout << "Could not rollback from frame " << currentFrame << "to frame " << lastConfirmedFrame << std::endl;
                }
                else if (rollbackCount > 0)
                {
                    //ToDo: will always rollback when last confirmed frame is < current frame. Add checking if the received input is equal to the predicted
                    //std::cout << "Rollback " << rollbackCount << " frames" << std::endl;

                    for(int32_t i = 0; i < rollbackCount; ++i)
                    {
                        worldManager.NextFrame(lastConfirmedFrame);
                        std::vector<Input*> inputs = clientHandler.GetAllClientInputs(basePhysicsWorld.GetCurrentFrame());
                        basePhysicsWorld.Update(deltaTime, inputs, clientHandler.GetClientID());
                    }
                }
            }
        }

        worldManager.NextFrame(lastConfirmedFrame);
        std::vector<Input*> inputs = clientHandler.GetAllClientInputs(currentFrame);
        basePhysicsWorld.Update(deltaTime, inputs, clientHandler.GetClientID());
        clientHandler.SendGameData(basePhysicsWorld);
    }

    void Render()
    {

        worldManager.GetPhysicsWorld().Render();
        glfwSwapBuffers(window);
    }

    void Serialize()
    {
        serializedStream.Clear();
        hasSerializedStream = true;
         worldManager.GetPhysicsWorld().Serialize(serializedStream);
    }

    void Deserialize()
    {
        Stream temporaryStream = Stream(serializedStream);
        worldManager.GetPhysicsWorld().Deserialize(temporaryStream);
        worldManager.Reset();
    }

private:
    static void SetFrameSize(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

private:
    WorldManager worldManager;

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