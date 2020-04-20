cmake_minimum_required(VERSION 3.10)

project("Mandlebrot Explorer Src")

add_compile_options(-Wall -Werror -pedantic -Wextra -O3 -flto)

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/src/config.cpp)
    add_custom_command(
        OUTPUT  ${CMAKE_SOURCE_DIR}/src/config.cpp
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/config.cpp.def ${CMAKE_SOURCE_DIR}/src/config.cpp
        DEPENDS ${CMAKE_SOURCE_DIR}/src/config.cpp.def
    )
endif()

add_library(config    config.cpp)
add_library(rendering rendering.cpp)
add_executable(mandlebrot_explorer  mandlebrot_explorer.cpp)

if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(mandlebrot_explorer PUBLIC "-pthread")
endif()

if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(mandlebrot_explorer "${CMAKE_THREAD_LIBS_INIT}")
endif()

target_link_libraries(mandlebrot_explorer config rendering ${SDL2_LIBRARIES})
