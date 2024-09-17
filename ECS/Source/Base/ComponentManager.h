#pragma once

#include "ComponentCollection.h"

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
		_componentArrays.insert({ typeName, std::make_shared < ComponentCollection<T>>() });
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
        GetComponentCollection<T>()->AddComponent(entity, component);
	}

    //Removes the component of type T from the given entity
    template<typename T>
    void RemoveComponent(Entity entity)
    {
        GetComponentCollection<T>()->RemoveComponent(entity);
    }

    //Gets a reference to the component of type T for the given entity
    template<typename T>
	T& GetComponent(Entity entity)
	{
		return GetComponentCollection<T>()->GetComponent(entity);
	}

    //Checks whether the given entity has the component of type T
    template<typename T>
    bool HasComponent(Entity entity)
    {
        return GetComponentCollection<T>()->HasComponent(entity);
    }

    //Removes all components that are associated to the given entity
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

    //Gets the component collection for a specific component of type T
	template<typename T>
	std::shared_ptr<ComponentCollection<T>> GetComponentCollection()
	{
        //ToDO: redo this and remove the string usage
		const char* typeName = typeid(T).name();
		assert(_componentTypes.find(typeName) != _componentTypes.end() && "Component not yet registered");

		return std::static_pointer_cast<ComponentCollection<T>>(_componentArrays[typeName]);
	}
};