cmake_minimum_required(VERSION 3.16)
project(Saya)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0") 
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 1. Gather Dependencies once
find_package(Torch REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)
pkg_check_modules(SDL3_TTF REQUIRED sdl3-ttf)
pkg_check_modules(CATCH2 QUIET catch2-with-main)

# 2. Define Sources
file(GLOB_RECURSE ALL_SRC CONFIGURE_DEPENDS "src/*.cpp")

# 3. Build Main App
add_executable(Saya ${ALL_SRC})
target_link_libraries(Saya PRIVATE 
    ${TORCH_LIBRARIES} 
    ${SDL3_LIBRARIES} 
    ${SDL3_TTF_LIBRARIES}
)
target_include_directories(Saya PRIVATE 
    src/ 
    src/ReinforcementLearning/
    ${SDL3_INCLUDE_DIRS}
    ${SDL3_TTF_INCLUDE_DIRS}
)

# 4. Build Tester
set(TEST_SRC ${ALL_SRC})
list(REMOVE_ITEM TEST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_executable(tester tests/test.cpp ${TEST_SRC})
target_link_libraries(tester PRIVATE 
    ${TORCH_LIBRARIES} 
    ${SDL3_LIBRARIES} 
    ${SDL3_TTF_LIBRARIES}
    ${CATCH2_LIBRARIES}
)
target_include_directories(tester PRIVATE 
    src/ 
    src/ReinforcementLearning/
    ${SDL3_INCLUDE_DIRS}
    ${SDL3_TTF_INCLUDE_DIRS}
    ${CATCH2_INCLUDE_DIRS}
)

# 5. Speed Boost: Precompiled Headers
# Note: switched tester to use SDL_ttf header in PCH to speed up gui/Window.cpp compilation
target_precompile_headers(Saya PRIVATE <torch/torch.h> <SDL3/SDL.h>)
target_precompile_headers(tester PRIVATE <catch2/catch_all.hpp> <torch/torch.h> <SDL3_ttf/SDL_ttf.h>)
