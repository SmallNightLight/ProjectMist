#pragma once

#include "ECSSettings.h"
#include "TypeList.h"
#include "ComponentCollection.h"

#include <array>
#include <cassert>
#include <cstring>

template<typename... Component>
using ComponentList = TypeList<Component...>;

template<typename... Component>
constexpr auto GetComponentOffsets() //todo: gener
{
    std::array<std::size_t, sizeof...(Component)> result { };

    std::size_t offset = 0;
    std::size_t i = 0;

	((result[i++] = offset, offset += sizeof(ComponentCollection<Component>)), ...);

    return result;
}

template<typename ComponentList>
class ComponentManager;

//Manages all component collections and uses the component name for easy lookups
template<typename... Component>
class ComponentManager<ComponentList<Component...>>
{
private:
	using Components = ComponentList<Component...>;

public:
	ComponentManager()
	{
		(RegisterComponent<Component>(), ...);
	}

	~ComponentManager()
	{
		(GetComponentCollection<Component>()->~ComponentCollection<Component>(), ...);
	}

	inline void Overwrite(const ComponentManager& other)
	{
		std::memcpy(Data.data(), other.Data.data(), sizeof(Data));
	}

    //Gets the unique component type ID for the component type T
	template<typename T>
	inline static constexpr ComponentType GetComponentType()
	{
		static_assert(Contains_v<T, Components>, "Component T is not part of the specified components");
		return IndexOf_v<T, Components>;
	}

    //Adds the component of type T to the given entity
	template<typename T>
	inline T* AddComponent(Entity entity, T component)
	{
		static_assert(Contains_v<T, Components>, "Component T is not part of the specified components");
        return GetComponentCollection<T>()->AddComponent(entity, component);
	}

    //Removes the component of type T from the given entity
    template<typename T>
    inline void RemoveComponent(Entity entity)
    {
		static_assert(Contains_v<T, Components>, "Component T is not part of the specified components");
        GetComponentCollection<T>()->RemoveComponent(entity);
    }

    //Gets a reference to the component of type T for the given entity
    template<typename T>
	inline T& GetComponent(Entity entity)
	{
		static_assert(Contains_v<T, Components>, "Component T is not part of the specified components");
        return GetComponentCollection<T>()->GetComponent(entity);
	}

    //Checks whether the given entity has the component of type T
    template<typename T>
    inline bool HasComponent(Entity entity) const
    {
		static_assert(Contains_v<T, Components>, "Component T is not part of the specified components");
        return GetComponentCollection<T>()->HasComponent(entity);
    }

    //Removes all components that are associated to the given entity
	inline void DestroyEntity(Entity entity)
	{
		(DestroyEntityForComponent<Component>(entity), ...);
	}

	//Gets the component collection for a specific component of type T
	template<typename T>
	inline constexpr ComponentCollection<T>* GetComponentCollection()
	{
		static_assert(Contains_v<T, Components>, "Component T is not part of the specified components");
		return reinterpret_cast<ComponentCollection<T>*>(&Data[ComponentOffset<T>]);
	}

	inline static constexpr size_t GetComponentCount()
	{
		return ComponentCount;
	}

private:
	template<typename T>
	void RegisterComponent()
	{
		ComponentCollection<T>* collection = new (&Data[ComponentOffset<T>]) ComponentCollection<T>();
		collection->Initialize();
	}

	template<typename T>
	void DestroyEntityForComponent(Entity entity)
	{
		reinterpret_cast<ComponentCollection<T>*>(&Data[ComponentOffset<T>])->DestroyEntity(entity);
	}

private:
	static constexpr size_t ComponentCount = Count_v<Components>;
	static constexpr size_t TotalSize = TotalSize_v<ComponentCollection<Component>...>; // TODO: might not work with s?

	static constexpr std::array<size_t, ComponentCount> Offsets = GetComponentOffsets<Component...>();

	template<typename T>
	static constexpr std::size_t ComponentOffset = Offsets[GetComponentType<T>()];

	std::array<uint8_t, TotalSize> Data;
};