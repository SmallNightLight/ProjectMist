set(CMAKE_TOOLCHAIN_FILE "C:/Matthias/Study/VStudio/LibariesCPP/vcpkg/scripts/buildsystems/vcpkg.cmake")

cmake_minimum_required(VERSION 3.27)
project(ECS)

set(CMAKE_CXX_STANDARD 17)

#find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

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
        Source/Math/FPM/fixed.hpp
        Source/Math/FPM/math.hpp
        Source/Math/FPM/ios.hpp
        Source/Math/FixedVector2.h
)

#target_link_libraries(ECS sfml-graphics sfml-window sfml-system)
target_link_libraries(ECS
        PRIVATE
        glfw
        OpenGL::GL
        GLEW::GLEW
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")