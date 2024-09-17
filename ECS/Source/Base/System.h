#pragma once

#include "Settings.h"

#include <set>

//Base class for all systems
//Gives access to a set of entities that the derived system can iterate over
class System
{
public:
	std::set<Entity> Entities;
};