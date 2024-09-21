cmake_minimum_required(VERSION 3.27)
project(Particles)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(Particles main.cpp
        Shader.h)

target_link_libraries(Particles
        PRIVATE
        glfw
        OpenGL::GL
        GLEW::GLEW
)

target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")