cmake_minimum_required(VERSION 3.10)

project("Mandlebrot Explorer Root" CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BINARY_DIR        ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH     ${CMAKE_BINARY_DIR})

find_package(SDL2 REQUIRED)
find_package(OpenMP REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)
