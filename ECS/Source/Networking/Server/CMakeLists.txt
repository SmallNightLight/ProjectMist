cmake_minimum_required(VERSION 3.27)
project(Server)

add_executable(Server main.cpp
        Server.h
        ../Shared/Log.h
        ../Shared/InputPacket.h
)