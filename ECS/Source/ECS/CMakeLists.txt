cmake_minimum_required(VERSION 3.27)
project(Physics LANGUAGES CXX)

add_library(ECS INTERFACE
        ECS.h
        ECSSettings.h
        TypeList.h
        EntityQueue.h
        EntitySet.h
        EntityPair.h
        EntityManager.h
        ComponentCollection.h
        ComponentManager.h
        SystemManager.h
        Layer.h
)