FILE(GLOB SOURCES *.cpp)

add_executable(test_main test_main.cpp)

target_sources(test_main PRIVATE ${SOURCES})

target_link_libraries(test_main antlang Boost::unit_test_framework)

add_custom_target(test test_main)
