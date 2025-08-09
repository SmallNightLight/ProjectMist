#pragma once

#include "ECSSettings.h"
#include "TypeList.h"
#include "ComponentManager.h"

#include <array>
#include <cassert>

template<typename... System>
using SystemList = TypeList<System...>;

template<typename ComponentList, typename SystemList>
class SystemManager;

//Manages the systems, their signatures and gives functionality to automatically adding components to a system when their signature match (or not)
template<typename... Component, typename... System>
class SystemManager<ComponentList<Component...>, SystemList<System...>>
{
private:
	using Components = ComponentList<Component...>;
	using Systems = SystemList<System...>;
	using Signature = std::bitset<Count_v<Components>>;

public:
	explicit SystemManager(ComponentManager<Components>& componentManager)
	{
		(RegisterSystem<System>(componentManager), ...);
	}

	~SystemManager()
	{
		(GetSystem<System>()->~System(), ...);
	}

	void Overwrite(const SystemManager& other)
	{
		(OverrideSystem<System>(other), ...);
	}

	template<typename T>
	T* GetSystem()
	{
		static_assert(Contains_v<T, Systems>, "System T is not part of the specified systems");
		return reinterpret_cast<T*>(Data.data() + SystemOffset<T>);
	}

	template<typename T>
	const T* GetSystem() const
	{
		static_assert(Contains_v<T, Systems>, "System T is not part of the specified systems");
		return reinterpret_cast<const T*>(Data.data() + SystemOffset<T>);
	}

    //Removes the given entity from all systems that have a reference to the entity
	void DestroyEntity(Entity entity)
	{
		(DestroyEntityForSystem<System>(entity), ...);
	}

    //Compares the new entity signature to all system signatures
    //When the signatures match the entity gets added to the system entity set
    //When the signatures do not match the entity gets removes from the system entity set
	void EntitySignatureChanged(Entity entity, Signature newSignature) //TODO: test with random removals
	{
		(EntitySignatureChangedForSystem<System>(entity, newSignature), ...);
	}

	template<typename T>
	inline static constexpr SystemType GetSystemType()
	{
		static_assert(Contains_v<T, Systems>, "System T is not part of the specified systems");
		return IndexOf_v<T, Systems>;
	}

	template<typename T>
	inline static constexpr Signature GetSystemSignature()
	{
		static_assert(Contains_v<T, Systems>, "System T is not part of the specified systems");
		return SystemSignature<T>;
	}

private:
	template<typename T>
	void RegisterSystem(ComponentManager<Components>& componentManager)
	{
		new (&Data[SystemOffset<T>]) T(componentManager);
	}

	template<typename T>
	inline void OverrideSystem(const SystemManager& other)
	{
		GetSystem<T>()->Entities = other.GetSystem<T>()->Entities;
	}

	template<typename T>
	inline void DestroyEntityForSystem(Entity entity)
	{
		GetSystem<T>()->Entities.Erase(entity);
	}

	template<typename T>
	inline void EntitySignatureChangedForSystem(Entity entity, Signature newSignature)
	{
		T* system = GetSystem<T>();
		constexpr Signature signature = SystemSignature<T>;

		if ((newSignature & signature) == signature)
		{
			//Entity signature matches system signature - insert into set
			system->Entities.Insert(entity);
		}
		else
		{
			//Entity signature does not match system signature - erase from set
			system->Entities.Erase(entity);
		}
	}

private:
	static constexpr ComponentType ComponentCount = Count_v<Components>;
	static constexpr SystemType SystemCount = Count_v<Systems>;
	static constexpr size_t TotalSize = TotalSize_v<System...>;
	static constexpr std::array<size_t, SystemCount> Offsets = GetOffsets<System...>();

	template<typename T>
	static constexpr std::size_t SystemOffset = Offsets[GetSystemType<T>()];

	template<typename List>
	struct SignatureHelper;

	template<typename... SignatureComponent>
	struct SignatureHelper<ComponentList<SignatureComponent...>>
	{
		static constexpr Signature Get()
		{
			Signature signature;
			(signature.set(ComponentManager<Components>::template GetComponentType<SignatureComponent>()), ...);
			return signature;
		}
	};

	template<typename T>
	static constexpr Signature SystemSignature =  SignatureHelper<typename T::RequiredComponents>::Get();

	std::array<uint8_t, TotalSize> Data;
};