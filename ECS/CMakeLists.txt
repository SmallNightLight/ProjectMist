if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
    set(CMAKE_TOOLCHAIN_FILE "C:/Matthias/Study/VStudio/LibariesCPP/vcpkg/scripts/buildsystems/vcpkg.cmake")
elseif(UNIX)
    if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
        set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "")
    endif()
    include("/home/matthias/Applications/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

cmake_minimum_required(VERSION 3.27)
project(Game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(Game Source/main.cpp
        Source/Rendering/Shader.h
        Source/Rendering/Camera.h

        Source/Game/Game.h
        Source/Game/GameSettings.h
        Source/Game/WorldManager.h
        Source/Game/World.h
        Source/Game/Worlds/PhysicsWorld.h
        Source/Game/Input/InputData.h
        Source/Game/Input/InputManager.h
        Source/Game/Input/Input.h
        Source/Game/Input/InputCollection.h
        Source/Game/Input/Input.cpp
        Source/Game/CacheManager.h
        Source/Game/Worlds/PhysicsWorldData.h
)

add_subdirectory(Source/ECS)
add_subdirectory(Source/Math)
add_subdirectory(Source/Physics)
add_subdirectory(Source/Networking/Client)
add_subdirectory(Source/Networking/Server)

include_directories(${CMAKE_SOURCE_DIR}/Includes)

if (WIN32)
    find_package(OpenGL REQUIRED)
endif()

find_package(glad CONFIG REQUIRED)
target_link_libraries(Game PRIVATE glad::glad)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(Game PRIVATE glfw)
find_package(glm CONFIG REQUIRED)
target_link_libraries(Game PRIVATE glm::glm)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif()