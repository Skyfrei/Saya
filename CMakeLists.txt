cmake_minimum_required(VERSION 3.16)
project(Saya LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")


# 1. Dependencies from Source
find_package(Torch REQUIRED)
add_subdirectory(SDL)
add_subdirectory(SDL/SDL_ttf)
add_subdirectory(Catch2)

# 2. Source Discovery
file(GLOB_RECURSE ALL_SRC CONFIGURE_DEPENDS "src/*.cpp")

# 3. Main App
add_executable(Saya ${ALL_SRC})
target_link_libraries(Saya PRIVATE ${TORCH_LIBRARIES} SDL3::SDL3 SDL3_ttf::SDL3_ttf)
target_include_directories(Saya PRIVATE src/ src/ReinforcementLearning/ SDL/include SDL/SDL_ttf/include)

# 4. Tester
set(TEST_SRC ${ALL_SRC})
list(REMOVE_ITEM TEST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
add_executable(tester tests/test.cpp ${TEST_SRC})
target_link_libraries(tester PRIVATE ${TORCH_LIBRARIES} SDL3::SDL3 SDL3_ttf::SDL3_ttf Catch2::Catch2WithMain)
target_include_directories(tester PRIVATE src/ src/ReinforcementLearning/ SDL/include SDL/SDL_ttf/include)

# 5. Precompiled Headers
target_precompile_headers(Saya PRIVATE <torch/torch.h> <SDL3/SDL.h>)
target_precompile_headers(tester PRIVATE <catch2/catch_all.hpp> <torch/torch.h> <SDL3_ttf/SDL_ttf.h>)

target_compile_options(tester PRIVATE -g -fno-omit-frame-pointer)
