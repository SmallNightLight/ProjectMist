#pragma once

#include "ComponentArray.h"

class ComponentManager
{
public:
	template<typename T>
	void RegisterComponent()
	{
		const char* typeName = typeid(T).name();

		assert(_componentTypes.find(typeName) == _componentTypes.end());

		//Add component type to component map
		_componentTypes.insert({ typeName, _nextComponentType });

		//Create componenetArray pointer and add it to the component array map
		_componentArrays.insert({ typeName, std::make_shared < SingleComponentArray<T>>() });

		_nextComponentType++;
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		const char* typeName = typeid(T).name();

		assert(_componentTypes.find(typeName) != _componentTypes.end() && "Component not yet registered");

		return _componentTypes[typeName];
	}

	template<typename T>
	void AddComponent(Entity entity, T component)
	{
		GetComponentArray<T>()->InsertData(entity, component);
	}

	template<typename T>
	T& GetComponent(Entity entity)
	{
		return GetComponentArray<T>()->GetData(entity);
	}

	void DestroyEntity(Entity entity)
	{
		for (auto const& pair : _componentArrays)
		{
			auto const& component = pair.second;
			component->DestroyEntity(entity);
		}
	}


private:
	std::unordered_map<const char*, ComponentType> _componentTypes{};
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> _componentArrays{};
	ComponentType _nextComponentType{};

	template<typename T>
	std::shared_ptr<SingleComponentArray<T>> GetComponentArray()
	{
		const char* typeName = typeid(T).name();
		assert(_componentTypes.find(typeName) != _componentTypes.end() && "Component not yet registered");

		return std::static_pointer_cast<SingleComponentArray<T>>(_componentArrays[typeName]);
	}
};