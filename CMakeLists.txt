cmake_minimum_required(VERSION 3.14)

project(antlang)

set(CMAKE_CXX_STANDARD 17)

find_package(doctest REQUIRED)

include_directories(antlang)

add_subdirectory(antlang)
add_subdirectory(tests)
add_subdirectory(programs)
