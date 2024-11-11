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


    World world;
};