cmake_minimum_required(VERSION 3.16)

# 1. Initialize Project
project(Saya CXX)

# 2. Set Standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 3. Compiler Flags (Warnings OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# 4. Find Dependencies (MUST BE DONE BEFORE add_executable)

# --- Torch ---
set(TORCH_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libtorch)
find_package(Torch REQUIRED PATHS ${TORCH_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

# --- SDL Configuration (MOVED UP) ---
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)
pkg_check_modules(SDL3_TTF REQUIRED sdl3-ttf)

# IMPORTANT: This tells the linker where to look for libraries
link_directories(${SDL3_LIBRARY_DIRS} ${SDL3_TTF_LIBRARY_DIRS})

# --- Catch2 (For Tester) ---
find_package(Catch2 3 REQUIRED)


# 5. Define Sources
file(GLOB_RECURSE CPPSRC "src/*.cpp")
file(GLOB_RECURSE SDLSRC "src/gui/*.cpp")


# 6. Build 'Saya' Executable
add_executable(Saya ${CPPSRC})

# PCH
target_precompile_headers(Saya PRIVATE <torch/torch.h>)

# Linking
target_link_libraries(Saya PRIVATE "${TORCH_LIBRARIES}")
target_link_libraries(Saya PRIVATE ${SDL3_LIBRARIES} ${SDL3_TTF_LIBRARIES})

# Includes
target_include_directories(Saya PRIVATE src/ReinforcementLearning/)
target_include_directories(Saya PRIVATE ${SDL3_INCLUDE_DIRS} ${SDL3_TTF_INCLUDE_DIRS})


# 7. Build 'Tester' Executable
# Remove main entry points from source lists
list(REMOVE_ITEM CPPSRC ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
list(REMOVE_ITEM SDLSRC ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/start.cpp)

add_executable(tester tests/test.cpp ${CPPSRC} ${SDLSRC})
target_precompile_headers(tester PRIVATE <torch/torch.h>)

# Linking Catch2 (Using standard Catch2::Catch2 since you define main)
target_link_libraries(tester PRIVATE Catch2::Catch2)
target_include_directories(tester PRIVATE ${CATCH2_INCLUDE_DIRS})

target_link_libraries(tester PRIVATE "${TORCH_LIBRARIES}")
target_link_libraries(tester PRIVATE ${SDL3_LIBRARIES} ${SDL3_TTF_LIBRARIES})
target_include_directories(tester PRIVATE ${SDL3_INCLUDE_DIRS} ${SDL3_TTF_INCLUDE_DIRS})
