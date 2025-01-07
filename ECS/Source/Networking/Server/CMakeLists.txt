cmake_minimum_required(VERSION 3.27)
project(Server)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
endif()

add_executable(Server main.cpp
        Server.h

        ../Shared/Log.h
        ../Shared/Packet.h
        ../Shared/InputPacket.h
        ../Shared/ThreadedQueue.h
        ../Shared/NetworkingSettings.h
        ../Shared/GameDataPacket.h
)

if(TARGET CLIENT)
    add_subdirectory(../../Math ${CMAKE_BINARY_DIR}/MathBuild)
endif()

#Asio
find_package(asio CONFIG REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries(Server PRIVATE asio::asio Threads::Threads)
target_compile_definitions(Server PRIVATE ASIO_STANDALONE)