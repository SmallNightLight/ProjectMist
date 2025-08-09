#pragma once

#include "ECSSettings.h"
#include "EntitySet.h"

//Base class for all systems
//Gives access to a set of entities that the derived system can iterate over
class System
{
public:
	System()
	{
		Entities.Initialize();
	}

	virtual ~System() = default;

public:
	EntitySet<MAXENTITIES> Entities;
};