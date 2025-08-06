#pragma once

#include "EntityQueue.h"
#include "ECSSettings.h"

#include <array>
#include <cassert>
#include <queue>
#include <cstring>

//Manages the entities and allows for their creation and destruction
//Saves the signature of each entity
class EntityManager
{
public:
    //Initializes the EntityManager by populating a queue with all available entities
	EntityManager()
	{
		for (Entity entity = 0; entity < MAXENTITIES; ++entity)
		{
			availableEntities.Push(entity);
		}
	}

	void Overwrite(const EntityManager& other)
	{
		std::memcpy(this, &other, sizeof(EntityManager));
	}

    //Creates a new entity and returns the entity ID
	Entity CreateEntity()
	{
		assert(activeEntityCount < MAXENTITIES && "Too many entities. Extend the buffer size");

		Entity id = availableEntities.Front();
		availableEntities.Pop();
		activeEntityCount++;

		return id;
	}

    //Destroys the entity and frees up the space for one additional entity
	void DestroyEntity(Entity entity)
	{
		assert(entity < MAXENTITIES && "Entity out of range");

		signatures[entity].reset();
		availableEntities.Push(entity);
		activeEntityCount--;
	}

    //Assigns a signature to the entity
	void SetSignature(Entity entity, Signature signature)
	{
		assert(entity < MAXENTITIES && "Entity out of range");

		signatures[entity] = signature;
	}

    //Gets the signature of the given entity
	Signature GetSignature(Entity entity) const
	{
		assert(entity < MAXENTITIES && "Entity out of range");

		return signatures[entity];
	}

	//Returns the count of all current active entities
	uint32_t GetEntityCount() const
	{
		return activeEntityCount;
	}

	//Gives a vector with all active entities that have at least one component that is included and its signature (excluding components that are not included)
	void GetActiveEntities(Signature includedComponents, std::vector<Entity>& entities, std::vector<Signature>& outSignatures) const
	{
		outSignatures.reserve(activeEntityCount);
		entities.reserve(activeEntityCount);

		for (Entity entity = 0; entity < MAXENTITIES; ++entity)
		{
			Signature signature = signatures[entity];

			if ((signature & includedComponents).any())
			{
				entities.push_back(entity);
				outSignatures.push_back(signature & includedComponents);
			}
		}
	}

private:
	uint32_t activeEntityCount { };
	std::array<Signature, MAXENTITIES> signatures { };
	EntityQueue<MAXENTITIES> availableEntities { };
};