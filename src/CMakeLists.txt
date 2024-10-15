cmake_minimum_required(VERSION 3.27)
project(chessEngineV2)

set(CMAKE_CXX_STANDARD 14)

add_executable(chessEngineV2 main.cpp
        Game.h
        constants.h
        Game.cpp
        Communication.cpp
        Engine.cpp)
