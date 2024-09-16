#pragma once

#include "Settings.h"

#include <array>
#include <cassert>
#include <unordered_map>
#include <bitset>
#include <memory>

class IComponentCollection
{
public:
	virtual ~IComponentCollection() = default;
	virtual void DestroyEntity(Entity entity) = 0;
};

/**
//Iteration 1: Bit-based ECS

//Issues: Components are not next to each other in memory, but spread out, based to the entity ID, inefficient for CPU caching

template<typename T>
class ComponentCollection : public IComponentCollection
{
public:
	void AddComponent(Entity entity, T component)
	{
        assert(entity < MAXENTITIES && "Entity out of range");
		assert(!_usedComponents[entity] && "Component added to same entity more then once. Use MultiComponentArray instead");

		_components[entity] = component;
		_usedComponents[entity] = true;
	}

	void RemoveComponent(Entity entity)
	{
        assert(entity < MAXENTITIES && "Entity out of range");
		assert(_usedComponents[entity] && "Removing a component that does not exist");

		_usedComponents[entity] = false;
	}

	T& GetComponent(Entity entity)
	{
        assert(entity < MAXENTITIES && "Entity out of range");
		assert(_usedComponents[entity] && "Trying to get a component that does not exist");

		return _components[entity];
	}

    bool HasComponent(Entity entity)
    {
        return _usedComponents[entity];
    }

	void DestroyEntity(Entity entity) override
	{
        assert(entity < MAXENTITIES && "Entity out of range");

        _usedComponents[entity] = false;
	}

private:
	std::array<T, MAXENTITIES> _components{};
	std::bitset<MAXENTITIES> _usedComponents{};
};
/**/

/**/
//Iteration 2: Sparse set-based ECS

//Issues: When removing components are removed the array reorders the entity indexes to make the array dense, resulting in a non-optimal order

template<typename T>
class ComponentCollection : public IComponentCollection
{
public:
    ComponentCollection()
    {
        _entityToIndex.fill(ENTITYNULL);
    }

    void AddComponent(Entity entity, T component)
    {
        assert(entity < MAXENTITIES && "Entity out of range");
        assert(_entityToIndex[entity] == ENTITYNULL&& "Component added to the same entity more than once. Use MultiComponentArray instead");

        //New index is the next available index in the component list
        std::int32_t entityIndex = _entityCount;

        //Update the maps and assign the component
        _entityToIndex[entity] = entityIndex;
        _indexToEntity[entityIndex] = entity;

        //Store the component
        _components[entityIndex] = component;
        _entityCount++;
    }

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

    T& GetComponent(Entity entity)
    {
        assert(entity < MAXENTITIES);
        assert(_entityToIndex[entity] != ENTITYNULL && "Trying to get a component that does not exist");
        return _components[_entityToIndex[entity]];
    }

    bool HasComponent(Entity entity)
    {
        return entity < MAXENTITIES && _entityToIndex[entity] != ENTITYNULL;
    }

    void DestroyEntity(Entity entity) override
    {
        assert(entity < MAXENTITIES && "Entity out of range");

        if (_entityToIndex[entity] != ENTITYNULL)
        {
            RemoveComponent(entity);
        }
    }

private:
    std::array<T, MAXENTITIES> _components { };
    std::array<Entity, MAXENTITIES> _indexToEntity { };
    std::array<std::int32_t, MAXENTITIES> _entityToIndex { };

    std::uint32_t _entityCount = 0;
};
/**/