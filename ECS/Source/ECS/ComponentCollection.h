#pragma once

#include "ECSSettings.h"

#include <array>
#include <cassert>

//Stores the components of type T in an array
//Sparse set-based ECS
//Issues: When removing components are removed the array reorders the entity indexes to make the array dense, resulting in a non-optimal order
template<typename T>
class ComponentCollection
{
	static_assert(std::is_trivially_default_constructible_v<T>, "ComponentCollection requires T to be default constructible and trivial");

public:
    inline ComponentCollection() noexcept = default;

    //Initializes the sparse set with null entities, to indicate that all entities have no components
    void Initialize()
    {
        entityToIndex.fill(ENTITYNULL);
        indexToEntity.fill(ENTITYNULL); //TODO can be removed also at the bottom
        entityCount = 0;
    }

    //Adds the component of type T to the given entity
    T* AddComponent(Entity entity, T component)
    {
        assert(entity < MAXENTITIES && "Entity out of range");
        assert(entityToIndex[entity] == ENTITYNULL&& "Component added to the same entity more than once. Use MultiComponentArray instead");

        //New index is the next available index in the component list
        std::int32_t entityIndex = entityCount;

        //Render the maps and assign the component
        entityToIndex[entity] = entityIndex;
        indexToEntity[entityIndex] = entity;

        //Store the component
        components[entityIndex] = component;
        entityCount++;

    	return &components[entityIndex];
    }

    //Removes the component from the given entity
    void RemoveComponent(Entity entity)
    {
        assert(entity < MAXENTITIES && "Entity out of range");
        assert(entityToIndex[entity] != ENTITYNULL && "Removing a component that does not exist");

        uint32_t indexOfRemovedEntity = entityToIndex[entity];
        uint32_t lastEntityIndex = entityCount - 1; //TODO: range exception

        //Move the last component to the index of the removed entity
        components[indexOfRemovedEntity] = components[lastEntityIndex];
        Entity entityOfLastIndex = indexToEntity[lastEntityIndex];

        //Update the sparse set
        entityToIndex[entityOfLastIndex] = indexOfRemovedEntity;
        indexToEntity[indexOfRemovedEntity] = entityOfLastIndex;

        //Set the now invalid index to NULL
        entityToIndex[entity] = ENTITYNULL;
        indexToEntity[lastEntityIndex] = ENTITYNULL;

        entityCount--;
    }

    //Gets a reference to the component for the given entity
    T& GetComponent(Entity entity)
    {
        assert(entity < MAXENTITIES);
        assert(entityToIndex[entity] != ENTITYNULL && "Trying to get a component that does not exist");
        return components[entityToIndex[entity]];
    }

    //Checks whether the given entity has the component by checking the sparse set for entity null
    bool HasComponent(Entity entity) const
    {
        return entity < MAXENTITIES && entityToIndex[entity] != ENTITYNULL;
    }

    //Returns the entity count (all entities that have this component type attached)
    std::uint32_t GetEntityCount() const
    {
        return entityCount;
    }

    //Removes the component from the entity if possible
    void DestroyEntity(Entity entity)
    {
        assert(entity < MAXENTITIES && "Entity out of range");

        if (entityToIndex[entity] != ENTITYNULL)
        {
            RemoveComponent(entity);
        }
    }

    void Overwrite(ComponentCollection* other)
    {
        std::memcpy(&components, &other->components, sizeof(std::array<T, MAXENTITIES>));
        indexToEntity = other->indexToEntity;
        entityToIndex = other->entityToIndex;
        entityCount = other->entityCount;
    }

private:
    std::array<T, MAXENTITIES> components;
    std::array<Entity, MAXENTITIES> indexToEntity;
    std::array<std::uint32_t, MAXENTITIES> entityToIndex;

    std::uint32_t entityCount;
};

static_assert(std::is_trivially_default_constructible_v<ComponentCollection<bool>>, "Component Collection needs to be trivial");