#pragma once

#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

class ECSManager
{
public:
    void Setup()
    {
        //Setup all managers
        _entityManager = std::make_unique<EntityManager>();
        _componentManager = std::make_unique<ComponentManager>();
        _systemManager = std::make_unique<SystemManager>();
    }

    //Entity methods
    Entity CreateEntity()
    {
        return _entityManager->CreateEntity();
    }

    void DestroyEntity(Entity entity)
    {
        _entityManager->DestroyEntity(entity);
        _componentManager->DestroyEntity(entity);
        _systemManager->DestroyEntity(entity);
    }

    //Component methods
    template<typename T>
    void RegisterComponent()
    {
        _componentManager->RegisterComponent<T>();
    }

    template<typename T>
    void AddComponent(Entity entity, T component)
    {
        _componentManager->AddComponent<T>(entity, component);
        Signature signature = _entityManager->GetSignature(entity);
        signature.set(_componentManager->GetComponentType<T>(), true);
        _entityManager->SetSignature(entity, signature);
        _systemManager->EntitySignatureChanged(entity, signature);
    }

    template<typename T>
    void RemoveComponent(Entity entity)
    {
        _componentManager->RemoveComponent<T>(entity);
        Signature  signature = _entityManager->GetSignature(entity);
        signature.set(_componentManager->GetComponentType<T>(), false);
        _entityManager->SetSignature(entity, signature);
        _systemManager->EntitySignatureChanged(entity, signature);
    }

    template<typename T>
    T& GetComponent(Entity entity)
    {
        return _componentManager->GetComponent<T>(entity);
    }

    template<typename T>
    bool HasComponent(Entity entity)
    {
        return _componentManager->HasComponent<T>(entity);
    }

    template<typename T>
    ComponentType GetComponentType()
    {
       return _componentManager->GetComponentType<T>();
    }

    //Systems methods
    template<typename T>
    std::shared_ptr<T> RegisterSystem()
    {
        return _systemManager->RegisterSystem<T>();
    }

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