#pragma once

#include "ECSSettings.h"
#include "System.h"

#include <memory>
#include <vector>
#include <cassert>

#include "Layer.h"

class Layer;

//Manages the systems, their signatures and gives functionality to automatically adding components to a system when their signature match (or not)
class SystemManager
{
public:
    //Registers the system of type T and sets the signature
	template<typename T>
	std::shared_ptr<T> RegisterSystem(Layer* world)
	{
		auto system = std::make_shared<T>(world);
		Signature signature = system->GetSignature();
		systems.emplace_back(signature, system);
		return system;
	}

    //Removes the given entity from all systems that have a reference to the entity
	void DestroyEntity(Entity entity)
	{
		for (auto& [signature, system] : systems)
		{
			system->Entities.erase(entity);
		}
	}

    //Compares the new entity signature to all system signatures
    //When the signatures match the entity gets added to the system entity set
    //When the signatures do not match the entity gets removes from the system entity set
	void EntitySignatureChanged(Entity entity, Signature newSignature)
	{
		for (auto& [signature, system] : systems)
		{
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
	std::vector<std::pair<Signature, std::shared_ptr<System>>> systems;
};