cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER_VERSION "10.5.0")

project(Final)

set(CPPSRC
./Map
./Race
./State
./Tools
Living.cpp)

set(PYLIB
./Bindings/Vec2.cpp)

set(PYBIND11_DIR /export/home/klaviot99dm/.local/lib/python3.8/site-packages)
find_package(pybind11 REQUIRED PATHS ${PYBIND11_DIR})
pybind11_add_module(pyl ${PYLIB} ${CPPSRC})

target_include_directories(pyl PRIVATE
        ./Map
        ./Race
        ./State
        ./Tools
        Living.cpp
)

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

target_link_libraries(pyl)