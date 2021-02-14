FILE(GLOB SOURCES *.cpp)

add_library(antlang)

target_sources(antlang PRIVATE ${SOURCES})
