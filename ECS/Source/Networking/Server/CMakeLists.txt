cmake_minimum_required(VERSION 3.27)
project(Server)

add_executable(Server main.cpp
        Server.h
        ../Shared/Log.h
        ../Shared/InputPacket.h

        ../../Game/Input/Input.h
        ../Shared/Stream.h
        ServerSettings.h
        UDPServer.h
)

#Asio
find_package(asio CONFIG REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries(Server PRIVATE asio::asio Threads::Threads)
target_compile_definitions(Server PRIVATE ASIO_STANDALONE)