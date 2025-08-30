cmake_minimum_required(VERSION 3.27)
project(Math LANGUAGES CXX)

add_library(Math INTERFACE
        FPM/fixed.hpp
        FPM/math.hpp
        FPM/ios.hpp

        FixedVector2.h
        Rect.h
        PartitionGrid2.h
        FixedTypes.h
        TestMath.h
        IntVector2.h
        FixedRandom.h
        FixedAABB.h
        Stream.h
        HashUtils.h
        Span.h
)