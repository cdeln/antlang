cmake_minimum_required(VERSION 3.6)

project(antstack)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-g)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

include_directories(antlang)

add_subdirectory(antlang)

add_subdirectory(programs)

add_subdirectory(tests)
