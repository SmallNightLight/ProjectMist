cmake_minimum_required(VERSION 3.27)
project(Math)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
endif()

add_executable(Math main.cpp
        FPM/fixed.hpp
        FPM/math.hpp
        FPM/ios.hpp
        FixedVector2.h
        Quadtree.h
        Rect.h
        PartitionGrid.h
        PartitionGrid2.h
        FixedTypes.h
        TestMath.h
        IntVector2.h
        FixedRandom.h
        AABB.h
        Stream.h

        TestMath.h
)

add_subdirectory(FixedLib)