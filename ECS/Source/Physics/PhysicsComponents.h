#pragma once

#include "Components/ColliderTransform.h"
#include "Components/RigidBodyData.h"
#include "Components/CircleCollider.h"
#include "Components/BoxCollider.h"
#include "Components/PolygonCollider.h"
#include "Components/ColliderRenderData.h"
#include "Components/Movable.h"

using PhysicsComponents = ComponentList<ColliderTransform, RigidBodyData, CircleCollider, BoxCollider, PolygonCollider, ColliderRenderData, Movable>;
using PhysicsComponentManager = ComponentManager<PhysicsComponents>;
using PhysicsLayer = Layer<PhysicsComponents>;