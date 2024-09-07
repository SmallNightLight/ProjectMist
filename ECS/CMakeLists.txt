set(CMAKE_TOOLCHAIN_FILE "C:/Matthias/Study/VStudio/LibariesCPP/vcpkg/scripts/buildsystems/vcpkg.cmake")

cmake_minimum_required(VERSION 3.27)
project(ECS)

set(CMAKE_CXX_STANDARD 17)

#maybe 2.6??
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

add_executable(ECS Source/main.cpp
        Source/Base/ComponentCollection.h
        Source/Base/ComponentManager.h
        Source/Base/EntityManager.h
        Source/Base/Settings.h
        Source/Base/System.h
        Source/Base/SystemManager.h
        Source/Base/ECSManager.h
        Source/Systems/Renderer.h
        Source/Components/RenderData.h
        Source/Components/Transform.h
        Source/Components/ComponentHeaders.h
        Source/Systems/SystemHeader.h
)

# Link SFML libraries to your executable
target_link_libraries(ECS sfml-graphics sfml-window sfml-system)