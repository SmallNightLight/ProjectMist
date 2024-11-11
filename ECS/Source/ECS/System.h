#pragma once

#include "ECSSettings.h"

#include <set>

class Layer;

//Base class for all systems
//Gives access to a set of entities that the derived system can iterate over
class System
{
public:
	explicit System(Layer* pLayer)
	{
		layer = pLayer;
	}

	virtual ~System() = default;

	[[nodiscard]] virtual Signature GetSignature() const = 0;

public:
	std::set<Entity> Entities;

protected:
	Layer* layer;
};