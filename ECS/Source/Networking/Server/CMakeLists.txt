cmake_minimum_required(VERSION 3.27)
project(Server)

add_executable(Server main.cpp
        Server.h
        UDPServer.h

        ../Shared/Log.h
        ../Shared/InputPacket.h
        ../Shared/Stream.h
        ../Shared/NetworkingSettings.h

        ../../Game/Input/Input.h
)

#Asio
find_package(asio CONFIG REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries(Server PRIVATE asio::asio Threads::Threads)
target_compile_definitions(Server PRIVATE ASIO_STANDALONE)