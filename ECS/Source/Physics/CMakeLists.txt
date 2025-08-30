cmake_minimum_required(VERSION 3.27)
project(Physics LANGUAGES CXX)

add_library(Physics INTERFACE
        Physics.h
        PhysicsSettings.h
        PhysicsUtils.h
        PhysicsComponents.h
        PhysicsSystems.h

        Additional/ColliderType.h
        Additional/RigidBodyType.h

        Collision/CollisionDetection.h
        Collision/CollisionCache.h
        Collision/unordered_dense.h
        Collision/ContactPair.h
        Collision/PhysicsCache.h

        Cache/SortedMap.h
        Cache/ComponentCollectionCache.h
        Cache/SortedDoubleSet.h
        Cache/SortedDoubleMap.h
        Cache/SortedMapArray.h

        Components/Transform.h
        Components/TransformMeta.h
        Components/RigidBodyData.h
        Components/BoxCollider.h
        Components/CircleCollider.h
        Components/PolygonCollider.h
        Components/ColliderRenderData.h
        Components/Movable.h

        Systems/RigidBody.h
        Systems/BoxColliderRenderer.h
        Systems/CircleColliderRenderer.h
        Systems/PolygonColliderRenderer.h
        Systems/MovingSystem.h
)