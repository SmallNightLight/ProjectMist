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

//Issues:
//Components not next to each other in memory, but spread out, based to the entity ID
//A lot of memory used for storing components

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

	T& GetData(Entity entity)
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


//Components should not be able to be added multiple times for an entity. An additive solution on one component is often better
/*
template<typename T>
//A multi component array - allows to store up to 255 components per entity. NOTE: First iterate and meanwhile keep track of the component index, Remove the component after each component iteration. When removing decrease the iterator by 1
class MultiComponentArray : public IComponentCollection
{
public:
	MultiComponentArray(byte maxComponentCount)
	{
		_maxComponentCount = maxComponentCount;
		_components = std::make_unique<T[]>(MAXENTITIES * _maxComponentCount);
	}

	void AddComponent(Entity entity, T component)
	{
		assert(_componentCount[entity] < 255 && "Component overflow - can't add more than 255 components");

		_components[entity * _maxComponentCount + _componentCount[entity]] = component;
		_componentCount[entity]++;
	}

	void RemoveComponent(Entity entity, byte componentIndex)
	{
		assert(_componentCount[entity] > componentIndex && "Trying to remove a component that is out of range");

		_components[entity * _maxComponentCount + componentIndex] = _components[entity * _maxComponentCount + _componentCount[entity] - 1];
		_componentCount[entity]--;
	}

	void DestroyEntity(Entity entity) override
	{
		_componentCount[entity] = 0;
	}

	//Methods to iterate over the components
	T* begin(Entity entity)
	{
		return _components.get() + entity * _maxComponentCount;
	}

	T* end(Entity entity)
	{
		return  _components.get() + entity * _maxComponentCount + _componentCount[entity];
	}

	const T* begin(Entity entity) const
	{
		return _components.get() + entity * _maxComponentCount;
	}

	const T* end(Entity entity) const
	{
		return  _components.get() + entity * _maxComponentCount + _componentCount[entity];
	}

private:
	std::unique_ptr<T[]> _components;
	std::array<byte, MAXENTITIES> _componentCount{};

	byte _maxComponentCount;
};*/