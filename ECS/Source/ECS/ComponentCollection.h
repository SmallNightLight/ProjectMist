#pragma once

#include "ECSSettings.h"

#include <array>
#include <cassert>
#include <bitset>
#include <memory>

//Interface for component collections, used to enforce the `DestroyEntity` method in all collections
class IComponentCollection
{
public:
	virtual ~IComponentCollection() = default;
	virtual void DestroyEntity(Entity entity) = 0;
    virtual void Overwrite(IComponentCollection* other) = 0;
};

//Stores the components of type T in an array
//Sparse set-based ECS
//Issues: When removing components are removed the array reorders the entity indexes to make the array dense, resulting in a non-optimal order
template<typename T>
class ComponentCollection : public IComponentCollection
{
	static_assert(std::is_default_constructible<T>::value, "ComponentCollection requires that T is default-constructible.");

public:
    //Initializes the sparse set with null entities, to indicate that all entities have no components
    ComponentCollection()
    {
        _entityToIndex.fill(ENTITYNULL);
        _indexToEntity.fill(ENTITYNULL); //Maybe not needed (in that case also remove the code for RemoveComponent)
    }

    //Adds the component of type T to the given entity
    T* AddComponent(Entity entity, T component)
    {
        assert(entity < MAXENTITIES && "Entity out of range");
        assert(_entityToIndex[entity] == ENTITYNULL&& "Component added to the same entity more than once. Use MultiComponentArray instead");

        //New index is the next available index in the component list
        std::int32_t entityIndex = _entityCount;

        //Render the maps and assign the component
        _entityToIndex[entity] = entityIndex;
        _indexToEntity[entityIndex] = entity;

        //Store the component
        _components[entityIndex] = component;
        _entityCount++;

    	return &_components[entityIndex];
    }

    //Removes the component from the given entity
    void RemoveComponent(Entity entity)
    {
        assert(entity < MAXENTITIES && "Entity out of range");
        assert(_entityToIndex[entity] != ENTITYNULL && "Removing a component that does not exist");

        std::int32_t indexOfRemovedEntity = _entityToIndex[entity];
        std::int32_t lastEntityIndex = _entityCount - 1;

        //Move the last component to the index of the removed entity
        _components[indexOfRemovedEntity] = _components[lastEntityIndex];
        Entity entityOfLastIndex = _indexToEntity[lastEntityIndex];

        //Update the sparse set
        _entityToIndex[entityOfLastIndex] = indexOfRemovedEntity;
        _indexToEntity[indexOfRemovedEntity] = entityOfLastIndex;

        //Set the now invalid index to NULL
        _entityToIndex[entity] = ENTITYNULL;
        _indexToEntity[lastEntityIndex] = ENTITYNULL;

        _entityCount--;
    }

    //Gets a reference to the component for the given entity
    T& GetComponent(Entity entity)
    {
        assert(entity < MAXENTITIES);
        assert(_entityToIndex[entity] != ENTITYNULL && "Trying to get a component that does not exist");
        return _components[_entityToIndex[entity]];
    }

    //Checks whether the given entity has the component by checking the sparse set for entity null
    bool HasComponent(Entity entity)
    {
        return entity < MAXENTITIES && _entityToIndex[entity] != ENTITYNULL;
    }

    //Removes the component from the entity if possible
    void DestroyEntity(Entity entity) override
    {
        assert(entity < MAXENTITIES && "Entity out of range");

        if (_entityToIndex[entity] != ENTITYNULL)
        {
            RemoveComponent(entity);
        }
    }

    void Overwrite(IComponentCollection* other) override
    {
        auto* collection = dynamic_cast<ComponentCollection<T>*>(other); //TODO: OR STATIC CAST??

        assert(!collection && "Types of components collection do not match");

        _components = collection->_components;
    }

private:
    std::array<T, MAXENTITIES> _components { };
    std::array<Entity, MAXENTITIES> _indexToEntity { };
    std::array<std::uint32_t, MAXENTITIES> _entityToIndex { };

    std::uint32_t _entityCount = 0;
};

//Fix for <brace-enclosed initializer list>:
//Missing default constructor with no parameters