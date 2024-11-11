#pragma once

#include "../World.h"
#include "../../Physics/Physics.h"

class PWorld
{
public:
    static constexpr void Register(World* world)
    {
        RegisterComponents(world);
        RegisterSystems(world);
    }

    static constexpr void RegisterComponents(World* world)
    {
        world->RegisterComponent<ColliderTransform>();
        world->RegisterComponent<RigidBodyData>();
        world->RegisterComponent<Movable>();
        world->RegisterComponent<ColliderRenderData>();
        world->RegisterComponent<CircleCollider>();
        world->RegisterComponent<BoxCollider>();
        world->RegisterComponent<PolygonCollider>();
    }

    static constexpr void RegisterSystems(World* world)
    {
        world->RegisterSystem<RigidBody>();
        world->RegisterSystem<MovingSystem>();
        world->RegisterSystem<CircleColliderRenderer>();
        world->RegisterSystem<BoxColliderRenderer>();
        world->RegisterSystem<PolygonColliderRenderer>();
    }
};