cmake_minimum_required(VERSION 3.27)
project(Client)

add_executable(Client main.cpp
        Client.h

        ../Shared/InputPacket.h
        ../Shared/Stream.h
        ../Shared/ThreadedQueue.h
        ../Shared/NetworkingSettings.h
        ../Shared/Log.h

        ../../Game/Input/Input.h
)

#Asio
find_package(asio CONFIG REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries(Client PRIVATE asio::asio Threads::Threads)
target_compile_definitions(Client PRIVATE ASIO_STANDALONE)