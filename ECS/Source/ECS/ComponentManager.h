#pragma once

#include "ECSSettings.h"
#include "ComponentCollection.h"

#include <unordered_map>
#include <array>
#include <cassert>
#include <memory>

//Manages all component collections and uses the component name for easy lookups
class ComponentManager
{
public:
	ComponentManager() = default;

	void Overwrite(const ComponentManager& other)
	{
		for(int i = 0; i < other.nextComponentType; ++i)
		{
			componentArrays[i]->Overwrite(other.componentArrays[i].get());
		}
	}

    //Registers the component of type T by using the type name as a hash and creates a new ComponentCollection filled with components of type T
	template<typename T>
	std::shared_ptr<ComponentCollection<T>> RegisterComponent()
	{
		const char* typeName = typeid(T).name();

		assert(componentTypes.find(typeName) == componentTypes.end() && "Component already registered"); //Todo: allow this case, since multiple worlds might want to register the same system

		//Add the component type to the component map
		componentTypes.insert({ typeName, nextComponentType });

		//Create a componentArray pointer and add it to the component array map
		auto componentCollection = std::make_shared<ComponentCollection<T>>();
		componentArrays[componentTypes[typeName]] = componentCollection;
		nextComponentType++;

		return componentCollection;
	}

    //Gets the unique component type ID for the component type T
	template<typename T>
	ComponentType GetComponentType()
	{
		const char* typeName = typeid(T).name();

		assert(componentTypes.find(typeName) != componentTypes.end() && "Component not yet registered");

		return componentTypes[typeName];
	}

    //Adds the component of type T to the given entity
	template<typename T>
	T* AddComponent(Entity entity, T component)
	{
        return GetComponentCollection<T>(GetComponentType<T>())->AddComponent(entity, component);
	}

	//Adds the component of type T to the given entity
	template<typename T>
	T* AddComponent(Entity entity, T component, ComponentType componentType)
	{
		return GetComponentCollection<T>(componentType)->AddComponent(entity, component);
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
		for(int i = 0; i < nextComponentType; ++i)
		{
			componentArrays[i]->DestroyEntity(entity);
		}
	}

	//Gets the component collection for a specific component of type T
	template<typename T>
	std::shared_ptr<ComponentCollection<T>> GetComponentCollection()
	{
		return GetComponentCollection<T>(GetComponentType<T>());
	}

	//Gets the component collection for a specific component of type T
	template<typename T>
	std::shared_ptr<ComponentCollection<T>> GetComponentCollection(ComponentType componentType)
	{
		assert(componentArrays[componentType] && "Component not yet registered");

		return std::static_pointer_cast<ComponentCollection<T>>(componentArrays[componentType]);
	}

private:
	std::unordered_map<const char*, ComponentType> componentTypes { };
    std::array<std::shared_ptr<IComponentCollection>, MAXCOMPONENTS> componentArrays { };
	ComponentType nextComponentType { };
};