#pragma once

#include "Settings.h"

#include <array>
#include <cassert>
#include <queue>

class EntityManager
{
public:
	EntityManager()
	{
		for (Entity entity = 0; entity < MAXENTITIES; ++entity)
		{
			_availableEntities.push(entity);
		}
	}

	Entity CreateEntity()
	{
		assert(_livingEntityCount < MAXENTITIES && "Too many entities. Extend the buffer size");

		Entity id = _availableEntities.front();
		_availableEntities.pop();
		_livingEntityCount++;

		return id;
	}

	void DestroyEntity(Entity entity)
	{
		assert(entity < MAXENTITIES && "Entity out of range");

		_signatures[entity].reset();
		_availableEntities.push(entity);
		_livingEntityCount--;
	}

	void SetSignature(Entity entity, Signature signature)
	{
		assert(entity < MAXENTITIES && "Entity out of range");

		_signatures[entity] = signature;
	}

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

//ToDo: Not compatible with MultiComponentArray (signatures)