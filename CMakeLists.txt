cmake_minimum_required(VERSION 3.6)

project(antstack)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-g -Wall)

option(SANITIZE OFF)

if(SANITIZE)
    add_compile_options(-fsanitize=address -fsanitize=undefined)
    add_link_options(-fsanitize=address -fsanitize=undefined)
endif()

find_package(doctest REQUIRED)

include_directories(antlang)

add_subdirectory(antlang)

add_subdirectory(programs)

add_subdirectory(tests)
