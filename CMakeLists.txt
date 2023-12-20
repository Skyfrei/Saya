cmake_minimum_required(VERSION 3.19)
project(Final)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_executable(Final
        main.cpp
        Living.h
        ./Tools
        ./State
        ./Race
        ./Map
        Race/Structure/Barrack.cpp
        Race/Unit/Peasant.cpp
        State/Player.cpp
        State/Manager.cpp
        State/Task.cpp
        State/Task.h
        Race/Unit/Unit.cpp
        Living.cpp
)

