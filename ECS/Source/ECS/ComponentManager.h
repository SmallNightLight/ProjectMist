#pragma once

#include "ECSSettings.h"
#include "ComponentCollection.h"

#include <array>
#include <cassert>
#include <cstring>
#include <utility>

template<typename... Components>
struct ComponentList { };

template<typename T, typename List>
struct IndexOf;

template<typename T, typename... Ts>
struct IndexOf<T, ComponentList<T, Ts...>> {
	static constexpr ComponentType value = 0;
};

template<typename T, typename U, typename... Ts>
struct IndexOf<T, ComponentList<U, Ts...>>
{
	static constexpr ComponentType value = 1 + IndexOf<T, ComponentList<Ts...>>::value;
};

template<typename T, typename List>
constexpr ComponentType IndexOf_v = IndexOf<T, List>::value;

template<typename T, typename List>
struct Contains;

template<typename T>
struct Contains<T, ComponentList<>> : std::false_type { };

template<typename T, typename... Ts>
struct Contains<T, ComponentList<T, Ts...>> : std::true_type { };

template<typename T, typename U, typename... Ts>
struct Contains<T, ComponentList<U, Ts...>> : Contains<T, ComponentList<Ts...>> { };

template<typename T, typename List>
constexpr bool Contains_v = Contains<T, List>::value;

template<typename... Components>
constexpr auto GetOffsets()
{
    std::array<std::size_t, sizeof...(Components)> result { };

    std::size_t offset = 0;
    std::size_t i = 0;

	((result[i++] = offset, offset += sizeof(ComponentCollection<Components>)), ...);

    return result;
}

template<typename ComponentList>
class ComponentManager;

//Manages all component collections and uses the component name for easy lookups
template<typename... Components>
class ComponentManager<ComponentList<Components...>>
{
public:
	ComponentManager()
	{
		(RegisterComponent<Components>(), ...);
	}

	~ComponentManager()
	{
		(GetComponentCollection<Components>()->~ComponentCollection<Components>(), ...);
	}

	inline void Overwrite(const ComponentManager& other)
	{
		std::memcpy(Data.data(), other.Data.data(), sizeof(Data));
	}

    //Gets the unique component type ID for the component type T
	template<typename T>
	inline static constexpr ComponentType GetComponentType()
	{
		static_assert(Contains_v<T, List>, "Component T is not part of the specified components");
		return IndexOf_v<T, List>;
	}

    //Adds the component of type T to the given entity
	template<typename T>
	inline T* AddComponent(Entity entity, T component)
	{
		static_assert(Contains_v<T, List>, "Component T is not part of the specified components");
        return GetComponentCollection<T>()->AddComponent(entity, component);
	}

    //Removes the component of type T from the given entity
    template<typename T>
    inline void RemoveComponent(Entity entity)
    {
		static_assert(Contains_v<T, List>, "Component T is not part of the specified components");
        GetComponentCollection<T>()->RemoveComponent(entity);
    }

    //Gets a reference to the component of type T for the given entity
    template<typename T>
	inline T& GetComponent(Entity entity)
	{
		static_assert(Contains_v<T, List>, "Component T is not part of the specified components");
        return GetComponentCollection<T>()->GetComponent(entity);
	}

    //Checks whether the given entity has the component of type T
    template<typename T>
    inline bool HasComponent(Entity entity) const
    {
		static_assert(Contains_v<T, List>, "Component T is not part of the specified components");
        return GetComponentCollection<T>()->HasComponent(entity);
    }

    //Removes all components that are associated to the given entity
	inline void DestroyEntity(Entity entity)
	{
		(DestroyEntityForComponent<Components>(entity), ...);
	}

	//Gets the component collection for a specific component of type T
	template<typename T>
	inline constexpr ComponentCollection<T>* GetComponentCollection()
	{
		static_assert(Contains_v<T, List>, "Component T is not part of the specified components");
		return reinterpret_cast<ComponentCollection<T>*>(&Data[ComponentOffset<T>]);
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
	using List = ComponentList<Components...>;

	static constexpr size_t ComponentCount = sizeof...(Components);
	static constexpr size_t TotalSize = (sizeof(ComponentCollection<Components>) + ... + 0);

	template<typename T>
	static constexpr size_t GetIndex() { return IndexOf_v<T, List>;}

	static constexpr std::array<size_t, ComponentCount> Offsets = GetOffsets<Components...>();

	template<typename T>
	static constexpr std::size_t ComponentOffset = Offsets[GetComponentType<T>()];

	std::array<uint8_t, TotalSize> Data;
};