cmake_minimum_required(VERSION 3.16)
project(Saya LANGUAGES CXX C)

# --- 1. Global Optimization ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")

# This speeds up dependency configuration significantly
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(CATCH_INSTALL_HELPERS OFF CACHE BOOL "" FORCE)

# --- 2. Dependencies ---
find_package(Torch REQUIRED)
add_subdirectory(SDL EXCLUDE_FROM_ALL)
add_subdirectory(SDL/SDL_ttf EXCLUDE_FROM_ALL)
add_subdirectory(Catch2 EXCLUDE_FROM_ALL)

# --- 3. Modular Source Discovery ---
# glob_recurse with configure_depends is slow. 
# We'll create a library target once to avoid re-parsing everything for 'tester'
file(GLOB_RECURSE SAYA_LIB_SRC "src/*.cpp")
list(REMOVE_ITEM SAYA_LIB_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(saya_core OBJECT ${SAYA_LIB_SRC})
target_include_directories(saya_core PUBLIC src/ src/ReinforcementLearning/ SDL/include SDL/SDL_ttf/include)

# --- 4. Targets ---
# Main App
add_executable(Saya src/main.cpp)
target_link_libraries(Saya PRIVATE saya_core ${TORCH_LIBRARIES} SDL3::SDL3 SDL3_ttf::SDL3_ttf)

# Tester (links to the same object files, no double-compiling!)
add_executable(tester tests/test.cpp)
target_link_libraries(tester PRIVATE saya_core ${TORCH_LIBRARIES} SDL3::SDL3 SDL3_ttf::SDL3_ttf Catch2::Catch2WithMain)

# --- 5. Optimized PCH ---
# We only apply PCH to the core library to speed up everything at once
target_precompile_headers(saya_core PRIVATE <torch/torch.h> <SDL3/SDL.h> <SDL3_ttf/SDL_ttf.h>)
