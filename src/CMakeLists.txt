cmake_minimum_required(VERSION 3.10)

project("Mandlebrot Explorer Src" CXX)

add_compile_options(-Wall -Werror -pedantic -Wextra -O3 -flto)

add_custom_command(
    OUTPUT  ${CMAKE_SOURCE_DIR}/src/config.cpp
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/config.cpp.def ${CMAKE_SOURCE_DIR}/src/config.cpp
    DEPENDS ${CMAKE_SOURCE_DIR}/src/config.cpp.def
)

add_library(config    config.cpp)
add_library(rendering rendering.cpp)
add_executable(mandlebrot_explorer  mandlebrot_explorer.cpp)

separate_arguments(OpenMP_CXX_FLAGS)
target_compile_options(mandlebrot_explorer PRIVATE ${OpenMP_CXX_FLAGS})
target_compile_options(rendering           PRIVATE ${OpenMP_CXX_FLAGS})

target_link_libraries(rendering SDL2::SDL2)
target_link_libraries(mandlebrot_explorer config rendering SDL2::SDL2 ${OpenMP_CXX_LIBRARIES})
