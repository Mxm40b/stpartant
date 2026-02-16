cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("stpartant")

find_package(raylib REQUIRED)

link_libraries(
  raylib)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(stpartant ${FLAGS} ${SOURCES})
