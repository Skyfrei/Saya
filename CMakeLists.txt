cmake_minimum_required(VERSION 3.16)
add_compile_options("-g")

project(Saya)
set(TORCH_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libtorch)


find_package(Torch REQUIRED PATHS ${TORCH_DIR})
set(CMAKE_CXX_FLAGS ${TORCH_CXX_FLAGS})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_subdirectory(SDL EXCLUDE_FROM_ALL)
file(GLOB_RECURSE CPPSRC
    "src/*.cpp"
    "src/*.h"
)
#set(CPPSRC src/main.cpp
#        src/Living.h
#        src/State/Graph.cpp
#        src/State/Map.cpp
#        src/State/Terrain.cpp
#        src/State/Terrain.h
#        src/Race/Structure/Barrack.cpp
#        src/Race/Unit/Peasant.cpp
#        src/Race/Unit/Unit.cpp
#        src/State/Manager.cpp
#        src/State/Manager.h
#        src/State/Player.h
#        src/State/Player.cpp
#        src/Living.cpp
#        src/ReinforcementLearning/RlManager.h
#        src/ReinforcementLearning/Transition.h
#        src/ReinforcementLearning/RlManager.cpp
#        src/ReinforcementLearning/DQN.h
#        src/ReinforcementLearning/DQN.cpp)
add_executable(Saya WIN32 ${CPPSRC})
#target_link_libraries(Saya PRIVATE SDL3::SDL3)
target_link_libraries(Saya PRIVATE "${TORCH_LIBRARIES}")
target_include_directories(Saya PRIVATE src/ReinforcementLearning/)

find_package(Catch2 3 REQUIRED)
# These tests can use the Catch2-provided main
add_executable(tester tests/test.cpp)
target_link_libraries(tester PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tester PRIVATE "${TORCH_LIBRARIES}")

