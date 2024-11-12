#pragma once

#include "../ECS/ECS.h"
#include "../Physics/Physics.h"

#include "../Components/ComponentHeaders.h"
#include "../Systems/SystemHeader.h"

#include "GameSettings.h"

#include "WorldManager.h"
#include "Worlds/PhysicsWorld.h"


class Game
{
public:
    Game() : worldManager(WorldManager())
    {
        physicsWorldType(worldManager.AddWorld<PhysicsWorld>());
    }

    void Setup()
    {
        std::mt19937 numberGenerator(12);

        Layer& layer = worldManager.GetCurrentLayer();


        Camera* camera = layer.AddComponent(layer.CreateEntity(), Camera(static_cast<Fixed16_16>(SCREEN_WIDTH), static_cast<Fixed16_16>(SCREEN_HEIGHT), Fixed16_16(20)));
    }

    void Update(GLFWwindow* window, Fixed16_16 deltaTime)
    {
        Layer& layer = worldManager.GetCurrentLayer();

        auto physicsWorld = worldManager.GetWorld<PhysicsWorld>(physicsWorldType);
        physicsWorld->Update(window, deltaTime);
    }

    void Render()

    {

    }

    WorldManager worldManager;
    WorldType physicsWorldType;
};