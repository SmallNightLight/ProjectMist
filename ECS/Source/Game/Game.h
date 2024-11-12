#pragma once

#include "../ECS/ECS.h"
#include "../Physics/Physics.h"

#include "GameSettings.h"

#include "World.h"
#include "Worlds/PWorld.h"


class Game
{
public:
    Game()
    {
        world = World();
        PWorld::Register(&world);
    }

    void Setup()
    {
        /*for (int i = 0; i < 15; ++i)
        {
            physicsWorld.CreateRandomCircle();
        }

        physicsWorld.AddComponent(10, Movable(Fixed16_16(20)));

        //Add boxes/
        for (int i = 0; i < 15; ++i)
        {
            physicsWorld.CreateRandomBox();
        }

        for (int i = 0; i < 15; ++i)
        {
            physicsWorld.CreateRandomPolygon();
        }*/
    }

    void Update()
    {

    }

    void Render()
    {

    }

    World world;
};