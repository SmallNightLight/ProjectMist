#pragma once

#include "../ECS/ECS.h"

#include "../Components/ComponentHeaders.h"

#include "GameSettings.h"

#include "WorldManager.h"
#include "Worlds/PhysicsWorld.h"

#include "Input/InputManager.h"

class Game
{
public:
    explicit Game(GLFWwindow& window) : worldManager(WorldManager()), input(InputManager(inputKeys))
    {
        InputManager<3>::SetupCallbacks(window);

        numberGenerator = std::mt19937(12);
        physicsWorldType = worldManager.AddWorld<PhysicsWorld>();


        Setup(window);
    }

    void Setup(GLFWwindow& window)
    {
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

    WorldManager worldManager;
    WorldType physicsWorldType;

    InputManager<3> input;
    static constexpr std::array<u_int16_t, 3> inputKeys = {GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT, GLFW_MOUSE_BUTTON_MIDDLE};

    std::mt19937 numberGenerator;
};