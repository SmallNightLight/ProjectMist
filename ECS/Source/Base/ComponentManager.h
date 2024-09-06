#pragma once

#include "ComponentCollection.h"

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

		//Create componentArray pointer and add it to the component array map
		_componentArrays.insert({ typeName, std::make_shared < ComponentCollection<T>>() });
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
        GetComponentCollection<T>()->AddComponent(entity, component);
	}

    template<typename T>
    void RemoveComponent(Entity entity)
    {
        GetComponentCollection<T>()->RemoveComponent(entity);
    }

    template<typename T>
	T& GetComponent(Entity entity)
	{
		return GetComponentCollection<T>()->GetData(entity);
	}

    template<typename T>
    bool HasComponent(Entity entity)
    {
        return GetComponentCollection<T>()->HasComponent(entity);
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
	std::unordered_map<const char*, std::shared_ptr<IComponentCollection>> _componentArrays{};
	ComponentType _nextComponentType{};

    //ToDO: redo this and remove the string usage
	template<typename T>
	std::shared_ptr<ComponentCollection<T>> GetComponentCollection()
	{
		const char* typeName = typeid(T).name();
		assert(_componentTypes.find(typeName) != _componentTypes.end() && "Component not yet registered");

		return std::static_pointer_cast<ComponentCollection<T>>(_componentArrays[typeName]);
	}
};