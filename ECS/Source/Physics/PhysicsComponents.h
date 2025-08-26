#pragma once

#include "Components/Transform.h"
#include "Components/TransformMeta.h"
#include "Components/RigidBodyData.h"
#include "Components/CircleCollider.h"
#include "Components/BoxCollider.h"
#include "Components/PolygonCollider.h"
#include "Components/ColliderRenderData.h"
#include "Components/Movable.h"

using PhysicsComponents = ComponentList<Transform, TransformMeta, RigidBodyData, CircleCollider, BoxCollider, PolygonCollider, ColliderRenderData, Movable>;