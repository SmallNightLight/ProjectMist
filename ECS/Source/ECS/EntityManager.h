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
		livingEntityCount = other.livingEntityCount;
		signatures = other.signatures;
		availableEntities = other.availableEntities;
	}

    //Creates a new entity and returns the entity ID
	Entity CreateEntity()
	{
		assert(livingEntityCount < MAXENTITIES && "Too many entities. Extend the buffer size");

		Entity id = availableEntities.front();
		availableEntities.pop();
		livingEntityCount++;

		return id;
	}

    //Destroys the entity and frees up the space for one additional entity
	void DestroyEntity(Entity entity)
	{
		assert(entity < MAXENTITIES && "Entity out of range");

		signatures[entity].reset();
		availableEntities.push(entity);
		livingEntityCount--;
	}

    //Assigns a signature to the entity
	void SetSignature(Entity entity, Signature signature)
	{
		assert(entity < MAXENTITIES && "Entity out of range");

		signatures[entity] = signature;
	}

    //Gets the signature of the given entity
	Signature GetSignature(Entity entity)
	{
		assert(entity < MAXENTITIES && "Entity out of range");

		return signatures[entity];
	}

private:
	uint32_t livingEntityCount { };
	std::array<Signature, MAXENTITIES> signatures { };
	std::queue<Entity> availableEntities { };
};