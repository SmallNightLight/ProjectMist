#pragma once

#include <GLFW/glfw3.h>

#include "../World.h"
#include "../../Physics/Physics.h"
#include "../Input/Input.h"
#include "../../Math/Stream.h"

class PhysicsWorld : public World
{
public:
    explicit PhysicsWorld(Layer& pLayer) : layer(pLayer)
    {
        RegisterComponents();
        RegisterSystems();

        camera = layer.AddComponent(layer.CreateEntity(), Camera(static_cast<Fixed16_16>(SCREEN_WIDTH), static_cast<Fixed16_16>(SCREEN_HEIGHT), Fixed16_16(20)));
    }

    explicit PhysicsWorld(Stream& stream, Layer& layer) : PhysicsWorld(layer)
    {
        DeserializeSystem<RigidBody>();
    }

    void RegisterComponents()
    {
        colliderTransformCollection =  layer.RegisterComponent<ColliderTransform>();
        rigidBodyDataCollection = layer.RegisterComponent<RigidBodyData>();
        circleColliderCollection = layer.RegisterComponent<CircleCollider>();
        boxColliderCollection = layer.RegisterComponent<BoxCollider>();
        polygonColliderCollection = layer.RegisterComponent<PolygonCollider>();
        colliderRenderDataCollection = layer.RegisterComponent<ColliderRenderData>();
        movableCollection = layer.RegisterComponent<Movable>();

        //REMOVE
        layer.RegisterComponent<Camera>();
    }

    void RegisterSystems()
    {
        rigidBodySystem = layer.RegisterSystem<RigidBody>();
        circleColliderRenderer = layer.RegisterSystem<CircleColliderRenderer>();
        boxColliderRenderer = layer.RegisterSystem<BoxColliderRenderer>();
        polygonColliderRenderer = layer.RegisterSystem<PolygonColliderRenderer>();
        movingSystem = layer.RegisterSystem<MovingSystem>();

        //REMOVE
        cameraSystem = layer.RegisterSystem<CameraSystem>();
    }

    void AddObjects(std::mt19937& numberGenerator)
    {
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

    void Update(Fixed16_16 deltaTime, std::vector<Input*>& inputs, std::mt19937& numberGenerator)
    {
        UpdateDebug(inputs, numberGenerator);

        Fixed16_16 stepTime = deltaTime / PhysicsIterations;

        for(int i = 0; i < PhysicsIterations; ++i)
        {
            rigidBodySystem->ApplyVelocity(stepTime);
            rigidBodySystem->DetectCollisions();
        }
    }

    void UpdateDebug(std::vector<Input*>& inputs, std::mt19937& numberGenerator)
    {
        for(Input* input : inputs)
        {
            if (input->GetKeyDown(GLFW_MOUSE_BUTTON_LEFT))
            {
                PhysicsUtils::CreateRandomCircleFromPosition(layer, numberGenerator, input->GetMousePosition(camera));
            }

            if (input->GetKeyDown(GLFW_MOUSE_BUTTON_RIGHT))
            {
                PhysicsUtils::CreateRandomBoxFromPosition(layer, numberGenerator, input->GetMousePosition(camera));
            }

            if (input->GetKeyDown(GLFW_MOUSE_BUTTON_MIDDLE))
            {
                PhysicsUtils::CreateRandomPolygonFromPosition(layer, numberGenerator, input->GetMousePosition(camera));
            }
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

    void Serialize(Stream& stream) const
    {
        //Write all systems
        SerializeSystem(stream, rigidBodySystem);
        SerializeSystem(stream, circleColliderRenderer);
        SerializeSystem(stream, boxColliderRenderer);
        SerializeSystem(stream, polygonColliderRenderer);
        SerializeSystem(stream, movingSystem);

        //Write all component data
        SerializeSystem(stream, colliderTransformCollection);
        SerializeSystem(stream, rigidBodyDataCollection);
        SerializeSystem(stream, circleColliderCollection);
        SerializeSystem(stream, boxColliderCollection);
        SerializeSystem(stream, polygonColliderCollection);
        SerializeSystem(stream, colliderRenderDataCollection);
        SerializeSystem(stream, movableCollection);
    }

    template<typename SystemType>
    static void SerializeSystem(Stream& stream, SystemType system)
    {
        //Write size of entities set
        stream.WriteInteger<Entity>(system->Entities.size());

        //Write all entities
        for (Entity entity : system->Entities)
        {
            stream.WriteInteger<Entity>(entity);
        }
    }

    template<typename SystemType, typename ComponentCollectionType>
    static void SerializeComponentCollection(Stream& stream, SystemType system, ComponentCollectionType componentCollection)
    {
        //Write component collection size (without unused buffer)
        stream.WriteInteger<uint32_t>(componentCollection->GetEntityCount());

        for(Entity entity : system.Entites)
        {
            componentCollection->GetComponent(entity).Serialize(stream);
        }
    }

    template<typename SystemType>
    void DeserializeSystem(Stream& stream, std::set<Entity>& entities)
    {
        //Read the entities that have the specified system
        Entity entityCount = stream.ReadInteger<Entity>();
        for (int i = 0; i < entityCount; ++i)
        {
            entities.insert(stream.ReadInteger<Entity>());
        }
    }

    template<typename SystemType, typename ComponentCollectionType>
    static void DeserializeComponentCollection(Stream& stream, SystemType system, ComponentCollectionType componentCollection, std::set<Entity>& entities)
    {
        componentCollection->AddComponent()
    }

private:
    Layer& layer;

    //Systems
    std::shared_ptr<RigidBody> rigidBodySystem;
    std::shared_ptr<CircleColliderRenderer> circleColliderRenderer;
    std::shared_ptr<BoxColliderRenderer> boxColliderRenderer;
    std::shared_ptr<PolygonColliderRenderer> polygonColliderRenderer;
    std::shared_ptr<MovingSystem> movingSystem;

    //Components
    std::shared_ptr<ComponentCollection<ColliderTransform>> colliderTransformCollection;
    std::shared_ptr<ComponentCollection<RigidBodyData>> rigidBodyDataCollection;
    std::shared_ptr<ComponentCollection<CircleCollider>> circleColliderCollection;
    std::shared_ptr<ComponentCollection<BoxCollider>> boxColliderCollection;
    std::shared_ptr<ComponentCollection<PolygonCollider>> polygonColliderCollection;
    std::shared_ptr<ComponentCollection<ColliderRenderData>> colliderRenderDataCollection;
    std::shared_ptr<ComponentCollection<Movable>> movableCollection;

    //REMOVE
    std::shared_ptr<CameraSystem> cameraSystem;
    Camera* camera;
};