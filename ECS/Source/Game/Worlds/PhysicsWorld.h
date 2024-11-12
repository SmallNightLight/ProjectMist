#pragma once

#include "../World.h"
#include "../../Physics/Physics.h"

class PhysicsWorld : World
{
public:
    explicit PhysicsWorld(Layer& pLayer) : layer(pLayer)
    {
        RegisterComponents();
        RegisterSystems();
    }

    void RegisterComponents()
    {
        layer.RegisterComponent<ColliderTransform>();
        layer.RegisterComponent<RigidBodyData>();
        layer.RegisterComponent<Movable>();
        layer.RegisterComponent<ColliderRenderData>();
        layer.RegisterComponent<CircleCollider>();
        layer.RegisterComponent<BoxCollider>();
        layer.RegisterComponent<PolygonCollider>();
    }

    void RegisterSystems()
    {
        rigidBodySystem = layer.RegisterSystem<RigidBody>();
        movingSystem = layer.RegisterSystem<MovingSystem>();
        circleColliderRenderer = layer.RegisterSystem<CircleColliderRenderer>();
        boxColliderRenderer = layer.RegisterSystem<BoxColliderRenderer>();
        polygonColliderRenderer = layer.RegisterSystem<PolygonColliderRenderer>();
    }

    void AddObjects()
    {
        for (int i = 0; i < 15; ++i)
        {
            PhysicsUtils::CreateRandomCircle(layer, numberGenerator, camera);
        }

        layer.AddComponent(10, Movable(Fixed16_16(20)));

        //Add boxes/
        for (int i = 0; i < 15; ++i)
        {
            PhysicsUtils::CreateRandomBox(layer, numberGenerator, camera);
        }

        for (int i = 0; i < 15; ++i)
        {
            PhysicsUtils::CreateRandomPolygon(layer, numberGenerator, camera);
        }
    }

    void Update(GLFWwindow* window, Fixed16_16 deltaTime)
    {
        UpdateDebug(window);

        Fixed16_16 stepTime = deltaTime / PhysicsIterations;

        for(int i = 0; i < PhysicsIterations; ++i)
        {
            movingSystem->Update(window, stepTime);

            rigidBodySystem->ApplyVelocity(stepTime);
            rigidBodySystem->DetectCollisions();
        }
    }

private:
    Layer& layer;

    std::shared_ptr<RigidBody> rigidBodySystem;
    std::shared_ptr<MovingSystem> movingSystem;
    std::shared_ptr<CircleColliderRenderer> circleColliderRenderer;
    std::shared_ptr<BoxColliderRenderer> boxColliderRenderer;
    std::shared_ptr<PolygonColliderRenderer> polygonColliderRenderer;
};