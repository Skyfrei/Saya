cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_COMPILER_VERSION "10.5.0")
set(CMAKE_BUILD_TYPE Debug)
add_compile_options("-g")

project(Final)

find_package(Torch REQUIRED PATHS ./libtorch/share/cmake/Torch)
set(CMAKE_CXX_FLAGS ${TORCH_CXX_FLAGS})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CPPSRC main.cpp
        Living.h
        State/Graph.cpp
        State/Map.cpp
        State/Terrain.cpp
        State/Terrain.h
        Race/Structure/Barrack.cpp
        Race/Unit/Peasant.cpp
        Race/Unit/Unit.cpp
        
        State/Manager.cpp
        State/Manager.h
        State/Player.h
        State/Player.cpp
        Living.cpp
        ReinforcementLearning/RlManager.h
        ReinforcementLearning/Transition.h
        ReinforcementLearning/RlManager.cpp
        ReinforcementLearning/DQN.h
        ReinforcementLearning/DQN.cpp)
#include_directories(./ include)
add_executable(Final ${CPPSRC})
target_precompile_headers(Final PRIVATE ReinforcementLearning/DQN.h)

#target_compile_options(Final PRIVATE "-ftime-trace")
target_link_libraries(Final "${TORCH_LIBRARIES}")
target_include_directories(Final PRIVATE ReinforcementLearning/DQN.h)



