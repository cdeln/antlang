cmake_minimum_required(VERSION 3.6)

project(antstack)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-g -Wall)

find_package(doctest REQUIRED)

include_directories(antlang)

add_subdirectory(antlang)

add_subdirectory(programs)

add_subdirectory(tests)
