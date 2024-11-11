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
			_availableEntities.push(entity);
		}
	}

    //Creates a new entity and returns the entity ID
	Entity CreateEntity()
	{
		assert(_livingEntityCount < MAXENTITIES && "Too many entities. Extend the buffer size");

		Entity id = _availableEntities.front();
		_availableEntities.pop();
		_livingEntityCount++;

		return id;
	}

    //Destroys the entity and frees up the space for one additional entity
	void DestroyEntity(Entity entity)
	{
		assert(entity < MAXENTITIES && "Entity out of range");

		_signatures[entity].reset();
		_availableEntities.push(entity);
		_livingEntityCount--;
	}

    //Assigns a signature to the entity
	void SetSignature(Entity entity, Signature signature)
	{
		assert(entity < MAXENTITIES && "Entity out of range");

		_signatures[entity] = signature;
	}

    //Gets the signature of the given component
	Signature GetSignature(Entity entity)
	{
		assert(entity < MAXENTITIES && "Entity out of range");

		return _signatures[entity];
	}

private:
	std::queue<Entity> _availableEntities{};
	std::array<Signature, MAXENTITIES> _signatures{};
	uint32_t _livingEntityCount{};
};