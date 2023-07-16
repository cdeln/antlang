add_executable(antpile antpile.cpp)

target_link_libraries(antpile antlang)

install(TARGETS antpile DESTINATION bin)
