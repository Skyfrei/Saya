cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER_VERSION "10.5.0")
set(CMAKE_BUILD_TYPE Debug)
add_compile_options("-g")
project(Final)

set(CPPSRC main.cpp
        Living.h
        Map/Graph.cpp
        Map/Map.cpp
        Race/Structure/Barrack.cpp
        Race/Unit/Peasant.cpp
        Race/Unit/Unit.cpp
        
        State/Manager.cpp
        State/Manager.h
        State/Player.h
        State/Player.cpp
        Living.cpp)
include_directories(./ include)

#set(PYLIB ./Bindings/bind.cpp)

#set(PYBIND11_DIR /export/home/klaviot99dm/.local/lib/python3.8/site-packages)
#find_package(pybind11 REQUIRED PATHS ${PYBIND11_DIR})

#pybind11_add_module(Warcraft ${PYLIB} ${CPPSRC})

add_executable(Final ${CPPSRC})
