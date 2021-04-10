file(GLOB SOURCES *.cpp)

add_executable(test_main EXCLUDE_FROM_ALL test_main.cpp)

target_sources(test_main PRIVATE ${SOURCES})

target_link_libraries(test_main antlang doctest::doctest)

add_custom_target(test test_main)
