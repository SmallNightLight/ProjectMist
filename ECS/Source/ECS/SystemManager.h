#pragma once

#include "ECSSettings.h"
#include "System.h"

#include <memory>
#include <cassert>

#include "Layer.h"

class Layer;

//Manages the systems, their signatures and gives functionality to automatically adding components to a system when their signature match (or not)
class SystemManager
{
public:
	SystemManager() = default;

	void Overwrite(const SystemManager& other)
	{
		for (unsigned int i = 0; i < other.systemCount; ++i)
		{
			auto system = systems[i].second;
			auto otherSystem = other.systems[i].second;

			//Only copy the entities - other data on the system will stay the same
			system->Entities = otherSystem->Entities;
		}
	}

    //Registers the system of type T and sets the signature
	template<typename T>
	std::shared_ptr<T> RegisterSystem(Layer* world)
	{
		auto system = std::make_shared<T>(world);
		Signature signature = system->GetSignature();
		systems[systemCount] = std::make_pair(signature, system);
		systemCount++;
		return system;
	}

	template<typename T>
	SystemType RegisterSystemType(Layer* world)
	{
		auto system = std::make_shared<T>(world);
		Signature signature = system->GetSignature();
		systems[systemCount] = std::make_pair(signature, system);
		systemCount++;
		return systemCount - 1;
	}

    //Removes the given entity from all systems that have a reference to the entity
	void DestroyEntity(Entity entity)
	{
		for (unsigned int i = 0; i < systemCount; ++i)
		{
			auto& [signature, system] = systems[i];
			system->Entities.erase(entity);
		}
	}

    //Compares the new entity signature to all system signatures
    //When the signatures match the entity gets added to the system entity set
    //When the signatures do not match the entity gets removes from the system entity set
	void EntitySignatureChanged(Entity entity, Signature newSignature)
	{
		for (unsigned int i = 0; i < systemCount; ++i)
		{
			auto& [signature, system] = systems[i];

			if ((newSignature & signature) == signature)
			{
				//Entity signature matches system signature - insert into set
				system->Entities.insert(entity);
			}
			else
			{
				//Entity signature does not match system signature - erase from set
				system->Entities.erase(entity);
			}
		}
	}

private:
	unsigned int systemCount = 0;
	std::array<std::pair<Signature, std::shared_ptr<System>>, MAXSYSTEMS> systems;
};