cmake_minimum_required(VERSION 3.21)
project(Netdbg)

set(CMAKE_CXX_STANDARD 20)

add_executable(Netdbg
        main.cpp
        network/Network.hpp
        network/Socket.hpp
        network/Utils.hpp
        network/Selector.hpp)
