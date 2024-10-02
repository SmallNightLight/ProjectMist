#pragma once

#include "ComponentCollection.h"

//#include <array>

//Manages all component collections and uses the component name for easy lookups
class ComponentManager
{
public:
    //Registers the component of type T by using the type name as a hash and creates a new ComponentCollection filled with components of type T
	template<typename T>
	void RegisterComponent()
	{
		const char* typeName = typeid(T).name();

		assert(_componentTypes.find(typeName) == _componentTypes.end());

		//Add the component type to the component map
		_componentTypes.insert({ typeName, _nextComponentType });

		//Create a componentArray pointer and add it to the component array map
		_componentArrays[_componentTypes[typeName]] = std::make_unique<ComponentCollection<T>>();
		_nextComponentType++;
	}

    //Gets the unique component type ID for the component type T
	template<typename T>
	ComponentType GetComponentType()
	{
		const char* typeName = typeid(T).name();

		assert(_componentTypes.find(typeName) != _componentTypes.end() && "Component not yet registered");

		return _componentTypes[typeName];
	}

    //Adds the component of type T to the given entity
	template<typename T>
	void AddComponent(Entity entity, T component)
	{
        int componentType = GetComponentType<T>();
        GetComponentCollection<T>(componentType)->AddComponent(entity, component);
	}

    //Removes the component of type T from the given entity
    template<typename T>
    void RemoveComponent(Entity entity)
    {
        int componentType = GetComponentType<T>();
        GetComponentCollection<T>(componentType)->RemoveComponent(entity);
    }

    //Gets a reference to the component of type T for the given entity
    template<typename T>
	T& GetComponent(Entity entity, ComponentType componentType)
	{
        return GetComponentCollection<T>(componentType)->GetComponent(entity);
	}

    //Checks whether the given entity has the component of type T
    template<typename T>
    bool HasComponent(Entity entity)
    {
        int componentType = GetComponentType<T>();
        return ComponentCollection<T>(componentType)->HasComponent(entity);
    }

    //Removes all components that are associated to the given entity
	void DestroyEntity(Entity entity)
	{
		for(int i = 0; i < 4; i++)
		{
			_componentArrays[i]->DestroyEntity(entity);
		}
	}

	//Gets the component collection for a specific component of type T
	template<typename T>
	ComponentCollection<T>* GetComponentCollection(ComponentType componentType)
	{
		assert(_componentArrays[componentType] && "Component not yet registered");

		return static_cast<ComponentCollection<T>*>(_componentArrays[componentType].get());
	}

	template<typename T>
	ComponentCollection<T>* GetComponentCollection()
	{
		ComponentType componentType = GetComponentType<T>();

		assert(_componentArrays[componentType] && "Component not yet registered");

		return static_cast<ComponentCollection<T>*>(_componentArrays[componentType].get());
	}

private:
	std::unordered_map<const char*, ComponentType> _componentTypes { };
    std::array<std::unique_ptr<IComponentCollection>, MAXCOMPONENTS> _componentArrays { };
	ComponentType _nextComponentType { };
};