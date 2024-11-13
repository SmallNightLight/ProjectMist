#pragma once

#include <GLFW/glfw3.h>

#include "../World.h"
#include "../../Physics/Physics.h"

class PhysicsWorld : public World
{
public:
    explicit PhysicsWorld(Layer& pLayer) : layer(pLayer)
    {
        RegisterComponents();
        RegisterSystems();

        numberGenerator = std::mt19937(12);
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

        //REMOVE
        layer.RegisterComponent<Camera>();
    }

    void RegisterSystems()
    {
        rigidBodySystem = layer.RegisterSystem<RigidBody>();
        movingSystem = layer.RegisterSystem<MovingSystem>();
        circleColliderRenderer = layer.RegisterSystem<CircleColliderRenderer>();
        boxColliderRenderer = layer.RegisterSystem<BoxColliderRenderer>();
        polygonColliderRenderer = layer.RegisterSystem<PolygonColliderRenderer>();

        //REMOVE
        cameraSystem = layer.RegisterSystem<CameraSystem>();
    }

    void AddObjects()
    {
        camera = layer.AddComponent(layer.CreateEntity(), Camera(static_cast<Fixed16_16>(SCREEN_WIDTH), static_cast<Fixed16_16>(SCREEN_HEIGHT), Fixed16_16(20)));

        //Add a ground
        PhysicsUtils::CreateBox(layer, Vector2(Fixed16_16(0), camera->Bottom), camera->Right - camera->Left + Fixed16_16(10), Fixed16_16(2), Static);

        //Add walls
        PhysicsUtils::CreateBox(layer, Vector2(camera->Left, Fixed16_16(0)), Fixed16_16(2), Fixed16_16(50), Static);
        PhysicsUtils::CreateBox(layer, Vector2(camera->Right, Fixed16_16(0)), Fixed16_16(2), Fixed16_16(50), Static);

        //Create rotated objects
        Entity e1 = PhysicsUtils::CreateBox(layer, Vector2(10, 0), Fixed16_16(25), Fixed16_16(1), Static);
        layer.GetComponent<ColliderTransform>(e1).Rotate(Fixed16_16(0, 1));

        Entity e2 = PhysicsUtils::CreateBox(layer, Vector2(-10, -10), Fixed16_16(25), Fixed16_16(1), Static);
        layer.GetComponent<ColliderTransform>(e2).Rotate(Fixed16_16(0, -1));

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
        //UpdateDebug(window);

        Fixed16_16 stepTime = deltaTime / PhysicsIterations;

        for(int i = 0; i < PhysicsIterations; ++i)
        {
            movingSystem->Update(window, stepTime);

            rigidBodySystem->ApplyVelocity(stepTime);
            rigidBodySystem->DetectCollisions();
        }
    }

    //REMOVE
    void Render() const
    {
        cameraSystem->Apply();

        circleColliderRenderer->Render();
        boxColliderRenderer->Render();
        polygonColliderRenderer->Render();

        //Debug
        /*if (RenderDebugInfo)
        {
            RenderDebugInfo(rigidBodySystem->collisionsRE);
        }*/

        if (RenderBoundingBoxes)
        {
            circleColliderRenderer->RenderAABB();
            boxColliderRenderer->RenderAABB();
            polygonColliderRenderer->RenderAABB();
        }
    }

private:
    Layer& layer;
    std::mt19937 numberGenerator;

    std::shared_ptr<RigidBody> rigidBodySystem;
    std::shared_ptr<MovingSystem> movingSystem;
    std::shared_ptr<CircleColliderRenderer> circleColliderRenderer;
    std::shared_ptr<BoxColliderRenderer> boxColliderRenderer;
    std::shared_ptr<PolygonColliderRenderer> polygonColliderRenderer;

    //REMOVE
    std::shared_ptr<CameraSystem> cameraSystem;
    Camera* camera;
};