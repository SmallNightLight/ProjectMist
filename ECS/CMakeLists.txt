set(CMAKE_TOOLCHAIN_FILE "C:/Matthias/Study/VStudio/LibariesCPP/vcpkg/scripts/buildsystems/vcpkg.cmake")

cmake_minimum_required(VERSION 3.27)
project(ECS)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(ECS Source/main.cpp
        Source/Base/ComponentCollection.h
        Source/Base/ComponentManager.h
        Source/Base/EntityManager.h
        Source/Base/Settings.h
        Source/Base/System.h
        Source/Base/SystemManager.h
        Source/Base/ECSManager.h
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
        Source/Math/Box.h
        Source/Math/Rect.h
        Source/Math/PartitionGrid.h
)

target_link_libraries(ECS
        PRIVATE
        glfw
        OpenGL::GL
        glad::glad
        glm::glm
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")