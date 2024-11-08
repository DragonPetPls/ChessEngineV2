cmake_minimum_required(VERSION 3.27)
project(chessEngineV2)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2")


add_executable(chessEngineV2 main.cpp
        Game.h
        constants.h
        Game.cpp
        Communication.cpp
        Engine.cpp
        magicBitboards.h
        magicBitboards.cpp
        engineConstants.h
        Evaluator.h
        Evaluator.cpp
        TranspositionTable.h
        TranspositionTable.cpp)
