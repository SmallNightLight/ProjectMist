#pragma once

#include "../ECS/ECS.h"

#include "../Components/ComponentHeaders.h"

#include "GameSettings.h"

#include "WorldManager.h"
#include "Worlds/PhysicsWorld.h"

class Game
{
public:
    Game() : worldManager(WorldManager())
    {
        physicsWorldType = worldManager.AddWorld<PhysicsWorld>();

        Setup();
    }

    void Setup()
    {
        worldManager.GetWorld<PhysicsWorld>(physicsWorldType)->AddObjects();
    }

    void Update(GLFWwindow* window, Fixed16_16 deltaTime)
    {
        worldManager.NextFrame();

        auto physicsWorld = worldManager.GetWorld<PhysicsWorld>(physicsWorldType);
        physicsWorld->Update(window, deltaTime);
    }

    void Render()
    {
        auto physicsWorld = worldManager.GetWorld<PhysicsWorld>(physicsWorldType);
        physicsWorld->Render();
    }

    WorldManager worldManager;
    WorldType physicsWorldType;
};