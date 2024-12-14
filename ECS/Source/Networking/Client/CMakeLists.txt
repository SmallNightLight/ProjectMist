cmake_minimum_required(VERSION 3.27)
project(Client)

add_executable(Client main.cpp
        ../Shared/Log.h
        ../Shared/InputPacket.h

        ../../Game/Input/Input.h
        ../Shared/Stream.h
        Client.h
)

#Asio
find_package(asio CONFIG REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries(Client PRIVATE asio::asio Threads::Threads)
target_compile_definitions(Client PRIVATE ASIO_STANDALONE)