if(WIN32)
    set(CMAKE_TOOLCHAIN_FILE "C:/Matthias/Study/VStudio/LibariesCPP/vcpkg/scripts/buildsystems/vcpkg.cmake")
elseif(UNIX)
    if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
        set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "")
    endif()
    include("/home/matthias/Applications/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

cmake_minimum_required(VERSION 3.27)
project(ECS)

set(CMAKE_CXX_STANDARD 17)

add_executable(ECS Source/main.cpp
        Source/ECS/ComponentCollection.h
        Source/ECS/ComponentManager.h
        Source/ECS/EntityManager.h
        Source/ECS/ECSSettings.h
        Source/ECS/System.h
        Source/ECS/SystemManager.h
        Source/ECS/Layer.h
        Source/Components/ComponentHeaders.h
        Source/Systems/SystemHeader.h
        Source/Math/FPM/fixed.hpp
        Source/Math/FPM/math.hpp
        Source/Math/FPM/ios.hpp
        Source/Math/FixedVector2.h
        Source/Components/Transform.h
        Source/Systems/Movement.h
        Source/Components/Velocity.h
        Source/Systems/ParticleRenderer.h
        Source/Rendering/Shader.h
        Source/Components/Boid.h
        Source/Systems/BoidMovement.h
        Source/Systems/BoidRenderer.h
        Source/Math/Quadtree.h
        Source/Math/Rect.h
        Source/Math/PartitionGrid.h
        Source/Math/PartitionGrid2.h
        Source/Components/Lifetime.h
        Source/Systems/BlinkingParticles.h
        Source/Math/FixedTypes.h
        Source/Math/TestMath.h
        Source/Math/IntVector2.h
        Source/Math/FixedRandom.h
        Source/Physics/Physics.h
        Source/Physics/Components/ColliderTransform.h
        Source/Physics/Components/RigidBodyData.h
        Source/Physics/Components/BoxCollider.h
        Source/Physics/Components/CircleCollider.h
        Source/Physics/PhysicsSettings.h
        Source/Physics/Components/ColliderRenderData.h
        Source/Physics/Systems/BoxColliderRenderer.h
        Source/Physics/Systems/CircleColliderRenderer.h
        Source/Physics/Systems/RigidBody.h
        Source/Components/Camera.h
        Source/Systems/CameraSystem.h
        Source/Physics/Components/Movable.h
        Source/Physics/Systems/MovingSystem.h
        Source/Physics/Collision/CollisionDetection.h
        Source/Physics/Collision/CollisionInfo.h
        Source/Physics/PhysicsWorld.h
        Source/Math/AABB.h
        Source/Physics/Components/PolygonCollider.h
        Source/Physics/Systems/PolygonColliderRenderer.h
        Source/Game/Game.h
        Source/Game/World.h
        Source/Game/GameSettings.h
        Source/ECS/ECS.h
        Source/Game/Worlds/PWorld.h

)

add_subdirectory(Source/Math/FixedLib)

include_directories(${CMAKE_SOURCE_DIR}/Includes)

if (WIN32)
    find_package(OpenGL REQUIRED)
endif()

find_package(glad CONFIG REQUIRED)
target_link_libraries(ECS PRIVATE glad::glad)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(ECS PRIVATE glfw)
find_package(glm CONFIG REQUIRED)
target_link_libraries(ECS PRIVATE glm::glm)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif()