#pragma once

#include "ECSSettings.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

#include <vector>

//Manages the different managers (EntityManager, ComponentManager and SystemManager)
//Has functionality for modifying the components, systems and signatures of entities

//Manages all component collections and uses the component name for easy lookups
template<typename ComponentList, typename SystemList>
class Layer;

template<typename... Component, typename... System>
class Layer<ComponentList<Component...>, SystemList<System...>>
{
    using Components = ComponentList<Component...>;
    using Systems = SystemList<System...>;
    using Signature = std::bitset<Count_v<Components>>;

public:
    Layer() :
        entityManager(EntityManager<ComponentCount>()),
        componentManager(ComponentManager<Components>()),
        systemManager(SystemManager<Components, Systems>(componentManager)),
        ignoreSignatureChanged(false) { }

    void Overwrite(const Layer& other)
    {
        assert(entitiesToDestroy.size() == 0 && "DestroyMarkedEntities() needs to be called first before overwriting the layer");

        entityManager.Overwrite(other.entityManager);
        componentManager.Overwrite(other.componentManager);
        systemManager.Overwrite(other.systemManager);
    }

    //Entity methods

    //Creates a new entity and returns the entity ID
    Entity CreateEntity()
    {
        return entityManager.CreateEntity();
    }

    //Marks the entity for destruction, destroy the marked entities later, when the component references have been dropped with DestroyMarkedEntities()
    void MarkEntityForDestruction(Entity entity)
    {
        entitiesToDestroy.push_back(entity);
    }

    //Destroys the entities that are marked and removes any components and systems that are related to the entities
    void DestroyMarkedEntities()
    {
        for (const Entity entity : entitiesToDestroy)
        {
            entityManager.DestroyEntity(entity);
            componentManager.DestroyEntity(entity);
            systemManager.DestroyEntity(entity);
        }

        entitiesToDestroy.clear();
    }

    //Unsafe method to instantly remove an entities with it components and systems
    void ImmediatelyDestroyEntity(Entity entity)
    {
        entityManager.DestroyEntity(entity);
        componentManager.DestroyEntity(entity);
        systemManager.DestroyEntity(entity);
    }

    //Returns the count of all current active entities
    uint32_t GetEntityCount() const
    {
        return entityManager.GetEntityCount();
    }

    //Gets the signature of the given entity
    Signature GetSignature(Entity entity) const
    {
        return entityManager.GetSignature(entity);
    }

    //Gives a vector with all active entities that have at least one component that is included and its signature (excluding components that are not included)
    void GetActiveEntities(Signature includedComponents, std::vector<Entity>& entities, std::vector<Signature>& signatures) const
    {
        return entityManager.GetActiveEntities(includedComponents, entities, signatures);
    }

    //Component methods

    //Adds the component to the given entity, updates the signature and updates on which systems the entity is registered based on the signature
    template<typename T>
    T* AddComponent(Entity entity, T component)
    {
        //Render the signature of the entity by including the new component
        Signature signature = entityManager.GetSignature(entity);
        signature.set(GetComponentType<T>(), true);
        entityManager.SetSignature(entity, signature);

        //Notify the system manager about the new signature
        if (!ignoreSignatureChanged)
        {
            systemManager.EntitySignatureChanged(entity, signature);
        }

        return componentManager.template AddComponent<T>(entity, component);
    }

    //Removes the component of type T from the entity, updates the signature and updates on which systems the entity is registered based on the signature
    template<typename T>
    void RemoveComponent(Entity entity)
    {
        componentManager.template RemoveComponent<T>(entity);

        //Render the signature of the entity by removing the component
        Signature signature = entityManager.GetSignature(entity);
        signature.set(componentManager.template GetComponentType<T>(), false);
        entityManager.SetSignature(entity, signature);

        //Notify the system manager about the new signature
        if (!ignoreSignatureChanged)
        {
            systemManager.EntitySignatureChanged(entity, signature);
        }
    }

    //Gets a reference to the component of type T for the given entity
    template<typename T>
    inline T& GetComponent(Entity entity)
    {
        return componentManager.template GetComponent<T>(entity);
    }

    template<typename T>
    inline constexpr ComponentCollection<T>* GetComponentCollection()
    {
        return componentManager.template GetComponentCollection<T>();
    }

    //Checks whether the given entity has the component of type T
    template<typename T>
    inline bool HasComponent(Entity entity) const
    {
        return componentManager.template HasComponent<T>(entity);
    }

    //Get the unique ComponentType ID for a component type T
    template<typename T>
    static constexpr ComponentType GetComponentType()
    {
       return ComponentManager<Components>::template GetComponentType<T>();
    }

    //Systems methods

    template<typename T>
    T* GetSystem()
    {
        return systemManager.template GetSystem<T>();
    }

    template<typename T>
    const T* GetSystem() const
    {
        return systemManager.template GetSystem<T>();
    }

    template<typename T>
    static constexpr Signature GetSystemSignature()
    {
        return SystemManager<Components, Systems>::template GetSystemSignature<T>();
    }

    //When batch-adding new systems (with ignoreSignatureChanged = true) call this function after to add all systems based on the current signature
    void FinalizeEntitySystems(Entity entity)
    {
        systemManager.EntitySignatureChanged(entity, entityManager.GetSignature(entity));
        ignoreSignatureChanged = false;
    }

    void IgnoreSignatureChanged(bool value)
    {
        ignoreSignatureChanged = value;
    }

private:
    static constexpr size_t ComponentCount = Count_v<Components>;

    EntityManager<ComponentCount> entityManager;
    ComponentManager<Components> componentManager;
    SystemManager<Components, Systems> systemManager;

    std::vector<Entity> entitiesToDestroy { };
    bool ignoreSignatureChanged;
};

//TODO: Ideas to improve:
// - don't always erase or insert into the system entity set in EntitySignatureChanged()
// - Archetype ECS