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

file (GLOB_RECURSE SDLSRC
    "src/gui/*.cpp"
    "src/gui/*.h"
)
find_package(PkgConfig REQUIRED)

set(SDLTTF_VENDORED ON)
pkg_check_modules(SDL3 REQUIRED sdl3)
pkg_check_modules(SDL3_TTF REQUIRED sdl3-ttf)
add_executable(sdl-test src/gui/start.cpp ${SDLSRC})
#target_link_libraries(sdl-test PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)

target_link_libraries(sdl-test PRIVATE ${SDL3_LIBRARIES} ${SDL3_TTF_LIBRARIES})
target_include_directories(sdl-test PRIVATE ${SDL3_INCLUDE_DIRS} ${SDL3_TTF_INCLUDE_DIRS})



list(REMOVE_ITEM CPPSRC ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
list(REMOVE_ITEM SDLSRC ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/start.cpp)
add_executable(tester  tests/test.cpp  ${CPPSRC} ${SDLSRC})

pkg_check_modules(CATCH2 QUIET catch2-with-main)
target_link_libraries(tester PRIVATE ${CATCH2_LIBRARIES})
target_include_directories(tester PRIVATE ${CATCH2_INCLUDE_DIRS})

target_link_libraries(tester PRIVATE "${TORCH_LIBRARIES}")

target_link_libraries(tester PRIVATE ${SDL3_LIBRARIES} ${SDL3_TTF_LIBRARIES})
target_include_directories(tester PRIVATE ${SDL3_INCLUDE_DIRS} ${SDL3_TTF_INCLUDE_DIRS})

#target_link_libraries(tester PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)


