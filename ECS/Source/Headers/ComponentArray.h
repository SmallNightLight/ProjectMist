#pragma once

#include "Settings.h"

#include <array>
#include <cassert>
#include <unordered_map>
#include <bitset>
#include <memory>

class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void DestroyEntity(Entity entity) = 0;
};

template<typename T>
class SingleComponentArray : public IComponentArray
{
public:
	void InsertData(Entity entity, T component) //TODO: assert entity out of range?
	{
		assert(!_usedComponents[entity] && "Component added to same entity more then once. Use MultiArray instead");

		_components[entity] = component;
		_usedComponents[entity] = true;
	}

	void RemoveData(Entity entity)
	{
		assert(_usedComponents[entity] && "Removing a component that does not exist");

		_usedComponents[entity] = false;
	}

	T& GetData(Entity entity)
	{
		assert(_usedComponents[entity] && "Trying to get a component that does not exist");

		return _components[entity];
	}

	void DestroyEntity(Entity entity) override
	{
		if (_components[entity])
		{
			RemoveData(entity);
		}
	}

private:
	std::array<T, MAXENTITIES> _components{};
	std::bitset<MAXENTITIES> _usedComponents{};
};


template<typename T>
//A multi component array - allows to store up to 255 components per entity. NOTE: First iterate and meanwhile keep track of the component index, Remove the component after each component iteration. When removing decrease the iterator by 1
class MultiComponentArray : public IComponentArray
{
public:
	MultiComponentArray(byte maxComponentCount)
	{
		_maxComponentCount = maxComponentCount;
		_components = std::make_unique<T[]>(MAXENTITIES * _maxComponentCount);
	}

	void AddComponent(Entity entity, T component) //TODO: assert entity out of range?
	{
		assert(_componentCount[entity] < 255 && "Component overflow - can't add more then 255 components");

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
};