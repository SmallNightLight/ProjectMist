#pragma once

#include "../Math/FixedTypes.h"
#include "../Math/AABB.h"
#include "PhysicsSettings.h"

#include "PhysicsComponents.h"
using PhysicsComponentManager = ComponentManager<PhysicsComponents>;

#include "PhysicsSystems.h"
using PhysicsLayer = Layer<PhysicsComponents, PhysicsSystems>;

#include "PhysicsUtils.h"