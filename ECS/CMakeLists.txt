cmake_minimum_required(VERSION 3.27)
project(ECS)

set(CMAKE_CXX_STANDARD 17)

add_executable(ECS Source/main.cpp
        Source/Headers/ComponentArray.h
        Source/Headers/ComponentManager.h
        Source/Headers/EntityManager.h
        Source/Headers/Settings.h
        Source/Headers/System.h
        Source/Headers/SystemManager.h
)
