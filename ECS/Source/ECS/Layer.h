#pragma once

#include "ECSSettings.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

#include <vector>

//Manages the different managers (EntityManager, ComponentManager and SystemManager)
//Has functionality for modifying the components, systems and signatures of entities
class Layer
{
public:
    Layer()
    {
        //Initializes the ECS managers (EntityManager, ComponentManager and SystemManager)
        entityManager = EntityManager();
        componentManager = ComponentManager();
        systemManager = SystemManager();
    }

    //friend void Overwrite(Layer& other);

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

    //Returns a vector of signatures of all active entities
    std::vector<Signature> GetActiveEntities() const
    {
        return entityManager.GetActiveSignatures();
    }

    //Component methods

    //Registers the component T to the component manager
    template<typename T>
    std::shared_ptr<ComponentCollection<T>> RegisterComponent()
    {
        return componentManager.RegisterComponent<T>();
    }

    //Adds the component to the given entity, updates the signature and updates on which systems the entity is registered based on the signature
    template<typename T>
    T* AddComponent(Entity entity, T component)
    {
        //Render the signature of the entity by including the new component
        Signature signature = entityManager.GetSignature(entity);
        signature.set(componentManager.GetComponentType<T>(), true);
        entityManager.SetSignature(entity, signature);

        //Notify the system manager about the new signature
        systemManager.EntitySignatureChanged(entity, signature);

        return componentManager.AddComponent<T>(entity, component);
    }

    //Removes the component of type T from the entity, updates the signature and updates on which systems the entity is registered based on the signature
    template<typename T>
    void RemoveComponent(Entity entity)
    {
        componentManager.RemoveComponent<T>(entity);

        //Render the signature of the entity by removing the component
        Signature  signature = entityManager.GetSignature(entity);
        signature.set(componentManager.GetComponentType<T>(), false);
        entityManager.SetSignature(entity, signature);

        //Notify the system manager about the new signature
        systemManager.EntitySignatureChanged(entity, signature);
    }

    //Gets a reference to the component of type T for the given entity
    template<typename T>
    T& GetComponent(Entity entity)
    {
        return componentManager.GetComponent<T>(entity, GetComponentType<T>());
    }

    template<typename T>
    ComponentCollection<T>* GetComponentCollection()
    {
        return componentManager.GetComponentCollection<T>();
    }

    template<typename T>
    ComponentCollection<T>* GetComponentCollection(ComponentType componentType)
    {
        return componentManager.GetComponentCollection<T>(componentType);
    }

    //Checks whether the given entity has the component of type T
    template<typename T>
    bool HasComponent(Entity entity)
    {
        return componentManager.HasComponent<T>(entity);
    }

    //Get the unique ComponentType ID for a component type T
    template<typename T>
    ComponentType GetComponentType()
    {
       return componentManager.GetComponentType<T>();
    }

    //Systems methods

    //Registers the system of type T to the system manager
    template<typename T>
    std::shared_ptr<T> RegisterSystem()
    {
        return systemManager.RegisterSystem<T>(this);
    }

    template<typename T>
    SystemType RegisterSystemType()
    {
        return systemManager.RegisterSystemType<T>(this);
    }

private:
    EntityManager entityManager;
    ComponentManager componentManager;
    SystemManager systemManager;

    std::vector<Entity> entitiesToDestroy { }; //bitset lol
};

//Ideas to improve:
// - when adding entities only call EntitySignatureChanged() when finished adding all components
// - don't always erase or insert into the system entity set in EntitySignatureChanged()
// - Archetype ECS