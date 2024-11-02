#pragma once

#include "Settings.h"

#include <set>

class ECSWorld;

//Base class for all systems
//Gives access to a set of entities that the derived system can iterate over
class System
{
public:
	explicit System(ECSWorld* world)
	{
		World = world;
	}


public:
	std::set<Entity> Entities;

protected:
	ECSWorld* World;
};