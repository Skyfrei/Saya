cmake_minimum_required(VERSION 3.16)
add_compile_options("-g")

project(Final)

find_package(Torch REQUIRED PATHS ./libtorch/share/cmake/Torch)
set(CMAKE_CXX_FLAGS ${TORCH_CXX_FLAGS})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CPPSRC src/main.cpp
        src/Living.h
        src/State/Graph.cpp
        src/State/Map.cpp
        src/State/Terrain.cpp
        src/State/Terrain.h
        src/Race/Structure/Barrack.cpp
        src/Race/Unit/Peasant.cpp
        src/Race/Unit/Unit.cpp
        src/State/Manager.cpp
        src/State/Manager.h
        src/State/Player.h
        src/State/Player.cpp
        src/Living.cpp
        src/ReinforcementLearning/RlManager.h
        src/ReinforcementLearning/Transition.h
        src/ReinforcementLearning/RlManager.cpp
        src/ReinforcementLearning/DQN.h
        src/ReinforcementLearning/DQN.cpp)
#include_directories(./ include)
add_executable(Final ${CPPSRC})
#target_precompile_headers(Final PRIVATE src/ReinforcementLearning/DQN.h)

#target_compile_options(Final PRIVATE "-ftime-trace")
target_link_libraries(Final "${TORCH_LIBRARIES}")
target_include_directories(Final PRIVATE src/ReinforcementLearning/)



