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
list(FILTER CPPSRC EXCLUDE REGEX ".*/src/gui/.*")
add_executable(Saya WIN32 ${CPPSRC})
#target_link_libraries(Saya PRIVATE SDL3::SDL3)
target_link_libraries(Saya PRIVATE "${TORCH_LIBRARIES}")
target_include_directories(Saya PRIVATE src/ReinforcementLearning/)

find_package(Catch2 3 REQUIRED)
# These tests can use the Catch2-provided main
list(REMOVE_ITEM CPPSRC ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
add_executable(tester tests/test.cpp ${CPPSRC})
target_link_libraries(tester PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tester PRIVATE "${TORCH_LIBRARIES}")


file (GLOB_RECURSE SDLSRC
    "src/gui/*.cpp"
    "src/gui/*.h"
)

add_subdirectory(SDL EXCLUDE_FROM_ALL)
add_executable(sdl-test src/gui/start.cpp ${SDLSRC})
target_link_libraries(sdl-test PRIVATE SDL3::SDL3)

