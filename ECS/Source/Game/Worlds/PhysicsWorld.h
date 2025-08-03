#pragma once

#include <GLFW/glfw3.h>

#include "../World.h"
#include "../../Physics/Physics.h"
#include "../Input/Input.h"
#include "../../Math/Stream.h"

#include <vector>
#include <array>
#include <random>

class PhysicsWorld : public World
{
public:
    explicit PhysicsWorld(Layer& layer) : baseLayer(layer), currentFrame(1), numberGenerator(std::mt19937(12))
    {
        RegisterComponents(layer);
        RegisterSystems(layer);
        InitializeCamera(layer);
    }

    ///Registers all components to the layer, sets the component collections and creates a signature which includes all components
    void RegisterComponents(Layer& layer)
    {
        colliderTransformCollection =  layer.RegisterComponent<ColliderTransform>();
        rigidBodyDataCollection = layer.RegisterComponent<RigidBodyData>();
        circleColliderCollection = layer.RegisterComponent<CircleCollider>();
        boxColliderCollection = layer.RegisterComponent<BoxCollider>();
        polygonColliderCollection = layer.RegisterComponent<PolygonCollider>();
        colliderRenderDataCollection = layer.RegisterComponent<ColliderRenderData>();
        movableCollection = layer.RegisterComponent<Movable>();
        layer.RegisterComponent<Camera>(); //Todo: Camera?

        //Create a signature that has all component flags
        includedComponents = 0;
        includedComponents.set(layer.GetComponentType<ColliderTransform>(), true);
        includedComponents.set(layer.GetComponentType<RigidBodyData>(), true);
        includedComponents.set(layer.GetComponentType<CircleCollider>(), true);
        includedComponents.set(layer.GetComponentType<BoxCollider>(), true);
        includedComponents.set(layer.GetComponentType<PolygonCollider>(), true);
        includedComponents.set(layer.GetComponentType<ColliderRenderData>(), true);
        includedComponents.set(layer.GetComponentType<Movable>(), true);
    }

    ///Registers all components to the layer
    static void RegisterComponentsForLayer(Layer& layer)
    {
        layer.RegisterComponent<ColliderTransform>();
        layer.RegisterComponent<RigidBodyData>();
        layer.RegisterComponent<CircleCollider>();
        layer.RegisterComponent<BoxCollider>();
        layer.RegisterComponent<PolygonCollider>();
        layer.RegisterComponent<ColliderRenderData>();
        layer.RegisterComponent<Movable>();
    }

    void RegisterSystems(Layer& layer)
    {
        rigidBodySystem = layer.RegisterSystem<RigidBody>();
        circleColliderRenderer = layer.RegisterSystem<CircleColliderRenderer>();
        boxColliderRenderer = layer.RegisterSystem<BoxColliderRenderer>();
        polygonColliderRenderer = layer.RegisterSystem<PolygonColliderRenderer>();
        movingSystem = layer.RegisterSystem<MovingSystem>();

        //REMOVE
        cameraSystem = layer.RegisterSystem<CameraSystem>();
    }

    static void RegisterSystemsForLayer(Layer& layer)
    {
        layer.RegisterSystem<RigidBody>();
        layer.RegisterSystem<CircleColliderRenderer>();
        layer.RegisterSystem<BoxColliderRenderer>();
        layer.RegisterSystem<PolygonColliderRenderer>();
        layer.RegisterSystem<MovingSystem>();
    }

    void InitializeCache(CacheManager* cache)
    {
        rigidBodySystem->InitializeCache(cache->GetCollisionCache());
    }

    void InitializeCamera(Layer& layer)
    {
        camera = layer.AddComponent(layer.CreateEntity(), Camera(static_cast<Fixed16_16>(SCREEN_WIDTH), static_cast<Fixed16_16>(SCREEN_HEIGHT), Fixed16_16(20)));
    }

    void OverwriteFrame(FrameNumber frame, std::mt19937 otherNumberGenerator)
    {
        currentFrame = frame;
        numberGenerator = otherNumberGenerator;
    }

    void AddObjects()
    {
        //Add a ground
        PhysicsUtils::CreateBox(baseLayer, Vector2(Fixed16_16(0), camera->Bottom), camera->Right - camera->Left + Fixed16_16(10), Fixed16_16(2), Static);

        //Add walls
        PhysicsUtils::CreateBox(baseLayer, Vector2(camera->Left, Fixed16_16(0)), Fixed16_16(2), Fixed16_16(50), Static);
        PhysicsUtils::CreateBox(baseLayer, Vector2(camera->Right, Fixed16_16(0)), Fixed16_16(2), Fixed16_16(50), Static);

        //Create rotated objects
        Entity e1 = PhysicsUtils::CreateBox(baseLayer, Vector2(10, 10), Fixed16_16(25), Fixed16_16(1), Static);
        baseLayer.GetComponent<ColliderTransform>(e1).Rotate(Fixed16_16(0, 1));

        Entity e2 = PhysicsUtils::CreateBox(baseLayer, Vector2(-10, 0), Fixed16_16(25), Fixed16_16(1), Static);
        baseLayer.GetComponent<ColliderTransform>(e2).Rotate(Fixed16_16(0, -1));

        for (int i = 0; i < 15; ++i)
        {
            PhysicsUtils::CreateRandomCircle(baseLayer, numberGenerator, camera);
        }

        baseLayer.AddComponent(10, Movable(Fixed16_16(20)));

        //Add boxes/
        for (int i = 0; i < 15; ++i)
        {
            PhysicsUtils::CreateRandomBox(baseLayer, numberGenerator, camera);
        }

        for (int i = 0; i < 15; ++i)
        {
            PhysicsUtils::CreateRandomPolygon(baseLayer, numberGenerator, camera);
        }
    }

    void Update(Fixed16_16 deltaTime, std::vector<Input*>& inputs, uint32_t id)
    {
        UpdateDebug(inputs);

        Fixed16_16 stepTime = deltaTime / PhysicsIterations;

        for (uint8_t i = 0; i < PhysicsIterations; ++i)
        {
            rigidBodySystem->ApplyVelocity(stepTime);
            rigidBodySystem->HandleCollisions(currentFrame, i, id);
        }

        ++currentFrame;
    }

    void UpdateDebug(std::vector<Input*>& inputs)
    {
        for (Input* input : inputs)
        {
            if (input->GetKeyDown(GLFW_MOUSE_BUTTON_LEFT))
            {
                PhysicsUtils::CreateRandomCircleFromPosition(baseLayer, numberGenerator, input->GetMousePosition(camera));
            }

            if (input->GetKeyDown(GLFW_MOUSE_BUTTON_RIGHT))
            {
                PhysicsUtils::CreateRandomBoxFromPosition(baseLayer, numberGenerator, input->GetMousePosition(camera));
            }

            if (input->GetKeyDown(GLFW_MOUSE_BUTTON_MIDDLE))
            {
                PhysicsUtils::CreateRandomPolygonFromPosition(baseLayer, numberGenerator, input->GetMousePosition(camera));
            }
        }
    }

    void Render() const
    {
        cameraSystem->Apply();

        circleColliderRenderer->Render();
        boxColliderRenderer->Render();
        polygonColliderRenderer->Render();

        //Debug
        if (PhysicsDebugMode)
        {
            RenderDebugInfo(rigidBodySystem->collisionsRE);

            circleColliderRenderer->RenderDebugOverlay();
            boxColliderRenderer->RenderDebugOverlay();
            polygonColliderRenderer->RenderDebugOverlay();
        }
    }

    FrameNumber GetCurrentFrame() const
    {
        return currentFrame;
    }

    std::mt19937 GetNumberGenerator() const
    {
        return numberGenerator;
    }

    //Serialization

    //First the entity count is written and then every entity ID with its signature
    //
    //Send entity for every send component construct signature here when adding the components
    //Disable automatic system adding during this time
    //At the end enable automatic system and call UpdateSystems() on set the signature on every entity
    //
    //Entities
    //Component ID - Entity Count - (Entity - ComponentData)
    //Systems have no data that needs to be sent

    void Serialize(Stream& stream) const
    {
        std::vector<Entity> entities;
        std::vector<Signature> signatures;

        //Write current frame
        stream.WriteInteger<FrameNumber>(currentFrame);

        //Write number generator
        SerializeGenerator(stream, numberGenerator);

        //Write the signatures of all active entities
        SerializeEntities(stream, entities, signatures);

        //Update bounding box and transformed vertices
        UpdateTransform(entities, signatures);

        //Write all component data
        SerializeComponentCollection<ColliderTransform>(stream, colliderTransformCollection, entities, signatures);
        SerializeComponentCollection<RigidBodyData>(stream, rigidBodyDataCollection, entities, signatures);
        SerializeComponentCollection<CircleCollider>(stream, circleColliderCollection, entities, signatures);
        SerializeComponentCollection<BoxCollider>(stream, boxColliderCollection, entities, signatures);
        SerializeComponentCollection<PolygonCollider>(stream, polygonColliderCollection, entities, signatures);
        SerializeComponentCollection<ColliderRenderData>(stream, colliderRenderDataCollection, entities, signatures);
        SerializeComponentCollection<Movable>(stream, movableCollection, entities, signatures);
    }

    ///Deserializes the stream and overwrites the layer data. Only use this methode in a try loop
    void Deserialize(Stream& stream)
    {
        //Create a new layer and then Overwrite the existing layer with the new layer
        Layer layer = Layer();
        RegisterComponentsForLayer(layer);
        RegisterSystemsForLayer(layer);
        layer.CreateEntity(); //Create an entity that would be the camera - essential for maintaining a correct entity queue

        std::vector<Entity> entities;
        std::vector<Signature> signatures;
        std::array<uint32_t, MAXENTITIES> entityIndexes;

        currentFrame = stream.ReadInteger<FrameNumber>();

        //Read the number generator
        DeserializeGenerator(stream, numberGenerator);

        //Read the signatures of all active entities
        DeserializeEntities(stream, entities, signatures);

        AddEntities(layer, entities, entityIndexes);
        layer.IgnoreSignatureChanged(true);

        //Write all component data
        DeserializeComponentCollection<ColliderTransform>(stream, layer, entityIndexes, signatures);
        DeserializeComponentCollection<RigidBodyData>(stream, layer, entityIndexes, signatures);
        DeserializeComponentCollection<CircleCollider>(stream, layer, entityIndexes, signatures);
        DeserializeComponentCollection<BoxCollider>(stream, layer, entityIndexes, signatures);
        DeserializeComponentCollection<PolygonCollider>(stream, layer, entityIndexes, signatures);
        DeserializeComponentCollection<ColliderRenderData>(stream, layer, entityIndexes, signatures);
        DeserializeComponentCollection<Movable>(stream, layer, entityIndexes, signatures);

        //Add the entities to the systems
        AddEntitiesToSystems(layer, entities, signatures);

        //Overwrite the layer with the new layer that holds the received data
        baseLayer.Overwrite(layer);
    }

private:

    // Save the state of the generator into the stream
    void SerializeGenerator(Stream& stream, const std::mt19937& generator) const
    {
        std::ostringstream oss;
        oss << generator;

        stream.WriteStream(oss);
    }

    ///Write the entities and their signatures to the stream //TODO: make all ///
    void SerializeEntities(Stream& stream, std::vector<Entity>& entities, std::vector<Signature>& signatures) const
    {
        baseLayer.GetActiveEntities(includedComponents, entities, signatures);

        Entity entityCount = entities.size();

        //Write the entity count
        stream.WriteInteger<Entity>(entityCount);

        //Write the entities and their signatures
        for (uint32_t i = 0; i < entityCount; ++i)
        {
            stream.WriteInteger(entities[i]);
            stream.WriteBitset<MAXCOMPONENTS>(signatures[i]);
        }
    }

    void UpdateTransform(const std::vector<Entity>& entities, const std::vector<Signature>& signatures) const
    {
        ComponentType circleColliderComponentType = baseLayer.GetComponentType<CircleCollider>();
        ComponentType boxColliderComponentType = baseLayer.GetComponentType<BoxCollider>();
        ComponentType polygonColliderComponentType = baseLayer.GetComponentType<PolygonCollider>();

        for (int i = 0; i < entities.size(); ++i)
        {
            Entity entity = entities[i];
            Signature signature = signatures[i];

            if (signature.test(circleColliderComponentType))
            {
                 colliderTransformCollection->GetComponent(entities[i]).OverrideTransformUpdateRequire(false);
            }

            if (signature.test(boxColliderComponentType))
            {
                auto boxCollider = boxColliderCollection->GetComponent(entity);
                colliderTransformCollection->GetComponent(entities[i]).GetTransformedVertices(boxCollider.TransformedVertices, boxCollider.Vertices);
            }

            if (signature.test(polygonColliderComponentType))
            {
                auto polygonCollider = polygonColliderCollection->GetComponent(entity);
                colliderTransformCollection->GetComponent(entities[i]).GetTransformedVertices(polygonCollider.TransformedVertices, polygonCollider.Vertices);
            }
        }
    }

    template<typename Component>
    void SerializeComponentCollection(Stream& stream, std::shared_ptr<ComponentCollection<Component>> componentCollection, const std::vector<Entity>& entities, const std::vector<Signature>& signatures) const
    {
        //Write the componentType
        ComponentType componentType = baseLayer.GetComponentType<Component>();
        Signature componentSignature = 0;
        componentSignature.set(componentType, true);
        stream.WriteInteger(componentType);

        //Write the entity count of the component collection
        stream.WriteInteger(componentCollection->GetEntityCount());

        //Write the entity and the component Data
        for (int i = 0; i < entities.size(); ++i)
        {
            if ((signatures[i] & componentSignature).any())
            {
                stream.WriteInteger(entities[i]);
                componentCollection->GetComponent(entities[i]).Serialize(stream);
            }
        }
    }

    //Deserialization

    static void DeserializeEntities(Stream& stream, std::vector<Entity>& entities, std::vector<Signature>& signatures)
    {
        //Read the entity count
        Entity entityCount = stream.ReadInteger<Entity>();

        if (entityCount > MAXENTITIES)
        {
            throw "Entity count larger then MAXENTITIES";
        }

        entities.resize(entityCount);
        signatures.resize(entityCount);

        //Read the entities and their signatures
        for (uint32_t i = 0; i < entityCount; ++i)
        {
            entities[i] = stream.ReadInteger<Entity>();
            signatures[i] = stream.ReadBitset<MAXCOMPONENTS>();
        }
    }

    static void DeserializeGenerator(Stream& stream, std::mt19937& generator)
    {
        std::istringstream iss = stream.ReadStream();
        iss >> generator;
    }

    //Add entities to the layer
    static void AddEntities(Layer& layer, const std::vector<Entity>& entities, std::array<uint32_t, MAXENTITIES>& entityIndexes)
    {
        for (Entity entity = 0; entity < MAXENTITIES; ++entity)
        {
            layer.CreateEntity();
        }

        std::bitset<MAXENTITIES> entityPresent;

        for (const Entity& entity : entities)
        {
            entityPresent.set(entity, true);
        }

        for (Entity entity = 0; entity < MAXENTITIES; ++entity)
        {
            if (!entityPresent.test(entity))
                layer.ImmediatelyDestroyEntity(entity);
        }

        //Create a list with the entity indexes
        for (uint32_t i = 0; i < entities.size(); ++i)
        {
            entityIndexes[entities[i]] = i;
        }
    }

    template<typename Component>
    static void DeserializeComponentCollection(Stream& stream, Layer& layer, std::array<uint32_t, MAXENTITIES>& entityIndexes, const std::vector<Signature>& signatures)
    {
        //Read the componentType and verify
        ComponentType componentType = stream.ReadInteger<ComponentType>();
        Signature componentSignature = 0;
        componentSignature.set(componentType, true);

        if (componentType != layer.GetComponentType<Component>())
        {
            throw "Component type mismatch";
        }

        //Read the entity count of the component collection
        uint32_t entityCount = stream.ReadInteger<uint32_t>();
        uint32_t signaturesCount = signatures.size();

        std::shared_ptr<ComponentCollection<Component>> componentCollection = layer.GetComponentCollection<Component>(componentType);

        //Read the entity and the component Data
        for (int i = 0; i < entityCount; ++i)
        {
            Entity entity = stream.ReadInteger<Entity>();

            if (entityCount > signaturesCount)
            {
                throw "Entity out of range of signatures";
            }

            if ((signatures[entityIndexes[entity]] & componentSignature).none())
            {
                throw "Signature does not match component type";
            }

            //Create new component from the stream using the specified constructor
            layer.AddComponent(entity, Component(stream), componentType, componentCollection);
        }
    }

    ///Adds the entities to the systems
    static void AddEntitiesToSystems(Layer& layer, const std::vector<Entity>& entities, const std::vector<Signature>& signatures)
    {
        for (uint32_t i = 0; i < entities.size(); ++i)
        {
            if (layer.GetSignature(entities[i]) != signatures[i])
            {
                throw "The received signature does not match the signature from the registered components";
            }

            layer.FinalizeEntitySystems(entities[i]);
        }
    }

    static void RenderDebugInfo(std::vector<CollisionInfo>& collisions)
    {
        for (CollisionInfo collision : collisions)
        {
            //Render normal of collision
            float normalLength = 1 + collision.Depth.ToFloating<float>();
            glLineWidth(2.0f);
            glColor3f(1.0f, 0.0f, 0.0f);
            glBegin(GL_LINES);
            glVertex2f(collision.Contact1.X.ToFloating<float>(), collision.Contact1.Y.ToFloating<float>());
            glVertex2f(collision.Contact1.X.ToFloating<float>() + collision.Normal.X.ToFloating<float>() * normalLength, collision.Contact1.Y.ToFloating<float>() + collision.Normal.Y.ToFloating<float>() * normalLength);
            glEnd();

            //Render contact points
            float size = 0.4f;
            if (collision.ContactCount > 0)
            {
                glLineWidth(2.0f);
                glColor3f(0.5f, 0.5f, 0.5f);
                glBegin(GL_LINES);
                glVertex2f(collision.Contact1.X.ToFloating<float>() - size, collision.Contact1.Y.ToFloating<float>() - size);
                glVertex2f(collision.Contact1.X.ToFloating<float>() + size, collision.Contact1.Y.ToFloating<float>() + size);
                glVertex2f(collision.Contact1.X.ToFloating<float>() + size, collision.Contact1.Y.ToFloating<float>() - size);
                glVertex2f(collision.Contact1.X.ToFloating<float>() - size, collision.Contact1.Y.ToFloating<float>() + size);
                glEnd();
            }
            if (collision.ContactCount > 1)
            {
                glLineWidth(2.0f);
                glBegin(GL_LINES);
                glVertex2f(collision.Contact2.X.ToFloating<float>() - size, collision.Contact2.Y.ToFloating<float>() - size);
                glVertex2f(collision.Contact2.X.ToFloating<float>() + size, collision.Contact2.Y.ToFloating<float>() + size);
                glVertex2f(collision.Contact2.X.ToFloating<float>() + size, collision.Contact2.Y.ToFloating<float>() - size);
                glVertex2f(collision.Contact2.X.ToFloating<float>() - size, collision.Contact2.Y.ToFloating<float>() + size);
                glEnd();
            }

            if (collision.ContactCount > 0)
            {
                if (collision.IsDynamic1)
                {
                    glColor3f(0.0f, 1.0f, 0.0f);
                }
                else
                {
                    glColor3f(0.0f, 0.0f, 1.0f);
                }

                glBegin(GL_POINTS);
                glVertex2f(collision.Contact1.X.ToFloating<float>(), collision.Contact1.Y.ToFloating<float>());
                glEnd();

                if (collision.IsDynamic2)
                {
                    glColor3f(0.0f, 1.0f, 0.0f);
                }
                else
                {
                    glColor3f(0.0f, 0.0f, 1.0f);
                }

                if (collision.ContactCount > 1)
                {
                    glBegin(GL_POINTS);
                    glVertex2f(collision.Contact2.X.ToFloating<float>(), collision.Contact2.Y.ToFloating<float>());
                    glEnd();
                }
            }
        }
    }

private:
    Layer& baseLayer;
    FrameNumber currentFrame;
    std::mt19937 numberGenerator;

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

    Signature includedComponents;

    //REMOVE
    std::shared_ptr<CameraSystem> cameraSystem;
    Camera* camera;
};