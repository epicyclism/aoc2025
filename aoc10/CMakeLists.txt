cmake_minimum_required (VERSION 3.21)

add_executable(aoc10 aoc10.cpp)
if(z3_FOUND)
    target_link_libraries(aoc10 PRIVATE ctre::ctre fmt::fmt z3::libz3)
else()
    target_link_libraries(aoc10 PRIVATE ctre::ctre fmt::fmt)
endif()


add_executable(aoc10_search aoc10_search.cpp)
target_link_libraries(aoc10_search PRIVATE ctre::ctre fmt::fmt)

add_executable(simplex simplex.cpp)
target_link_libraries(simplex PRIVATE ctre::ctre fmt::fmt)
