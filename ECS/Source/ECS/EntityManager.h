#pragma once

#include "ECSSettings.h"

#include <array>
#include <cassert>
#include <queue>

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
			availableEntities.push(entity);
		}
	}

	void Overwrite(const EntityManager& other)
	{
		activeEntityCount = other.activeEntityCount;
		signatures = other.signatures;
		availableEntities = other.availableEntities;
	}

    //Creates a new entity and returns the entity ID
	Entity CreateEntity()
	{
		assert(activeEntityCount < MAXENTITIES && "Too many entities. Extend the buffer size");

		Entity id = availableEntities.front();
		availableEntities.pop();
		activeEntityCount++;

		return id;
	}

    //Destroys the entity and frees up the space for one additional entity
	void DestroyEntity(Entity entity)
	{
		assert(entity < MAXENTITIES && "Entity out of range");

		signatures[entity].reset();
		availableEntities.push(entity);
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

	//Returns a vector of signatures of all active entities
	std::vector<Signature> GetActiveSignatures() const
	{
		std::vector<Signature> signatures;
		signatures.reserve(activeEntityCount);

		for (Entity entity = 0; entity < MAXENTITIES; ++entity)
		{
			if (signatures[entity].any())
			{
				signatures.push_back(GetSignature(entity));
			}
		}

		return signatures;
	}

private:
	uint32_t activeEntityCount { };
	std::array<Signature, MAXENTITIES> signatures { };
	std::queue<Entity> availableEntities { };
};