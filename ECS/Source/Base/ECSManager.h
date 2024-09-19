#pragma once

#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

//Manages the different managers (EntityManager, ComponentManager and SystemManager)
//Has functionality for modifying the components, systems and signatures of entities
class ECSManager
{
public:
    //Initializes the ECS managers (EntityManager, ComponentManager and SystemManager)
    void Setup()
    {
        //Setup all managers
        _entityManager = std::make_unique<EntityManager>();
        _componentManager = std::make_unique<ComponentManager>();
        _systemManager = std::make_unique<SystemManager>();
    }

    //Entity methods

    //Creates a new entity and returns the entity ID
    Entity CreateEntity()
    {
        return _entityManager->CreateEntity();
    }

    //Destroys the entity and removes any components and systems that are related to it
    void DestroyEntity(Entity entity)
    {
        _entityManager->DestroyEntity(entity);
        _componentManager->DestroyEntity(entity);
        _systemManager->DestroyEntity(entity);
    }

    //Component methods

    //Registers the component T to the component manager
    template<typename T>
    void RegisterComponent()
    {
        _componentManager->RegisterComponent<T>();
    }

    //Adds the component to the given entity, updates the signature and updates on which systems the entity is registered based on the signature
    template<typename T>
    void AddComponent(Entity entity, T component)
    {
        _componentManager->AddComponent<T>(entity, component);

        //Render the signature of the entity by including the new component
        Signature signature = _entityManager->GetSignature(entity);
        signature.set(_componentManager->GetComponentType<T>(), true);
        _entityManager->SetSignature(entity, signature);

        //Notify the system manager about the new signature
        _systemManager->EntitySignatureChanged(entity, signature);
    }

    //Removes the component of type T from the entity, updates the signature and updates on which systems the entity is registered based on the signature
    template<typename T>
    void RemoveComponent(Entity entity)
    {
        _componentManager->RemoveComponent<T>(entity);

        //Render the signature of the entity by removing the component
        Signature  signature = _entityManager->GetSignature(entity);
        signature.set(_componentManager->GetComponentType<T>(), false);
        _entityManager->SetSignature(entity, signature);

        //Notify the system manager about the new signature
        _systemManager->EntitySignatureChanged(entity, signature);
    }

    //Gets a reference to the component of type T for the given entity
    template<typename T>
    T& GetComponent(Entity entity, ComponentType componentType)
    {
        return _componentManager->GetComponent<T>(entity, componentType);
    }

    //Checks whether the given entity has the component of type T
    template<typename T>
    bool HasComponent(Entity entity)
    {
        return _componentManager->HasComponent<T>(entity);
    }

    //Get the unique ComponentType ID for a component type T
    template<typename T>
    ComponentType GetComponentType()
    {
       return _componentManager->GetComponentType<T>();
    }

    //Systems methods

    //Registers the system of type T to the system manager
    template<typename T>
    std::shared_ptr<T> RegisterSystem()
    {
        return _systemManager->RegisterSystem<T>();
    }

    //Set the signature (a mask indicating required components) for a system of type T
    template<typename T>
    void SetSignature(Signature signature)
    {
        _systemManager->SetSignature<T>(signature);
    }

private:
    std::unique_ptr<EntityManager> _entityManager;
    std::unique_ptr<ComponentManager> _componentManager;
    std::unique_ptr<SystemManager> _systemManager;
};