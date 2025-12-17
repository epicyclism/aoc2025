cmake_minimum_required (VERSION 3.21)

add_executable(aoc10_z3 aoc10_z3.cpp)
if(z3_FOUND)
    target_link_libraries(aoc10_z3 PRIVATE ctre::ctre fmt::fmt z3::libz3)
else()
    target_link_libraries(aoc10_z3 PRIVATE ctre::ctre fmt::fmt)
endif()

add_executable(aoc10 aoc10.cpp simplex.h simplex.cpp)
target_link_libraries(aoc10 PRIVATE ctre::ctre fmt::fmt)

add_executable(simplex2 simplex2.cpp simplex.cpp)
target_link_libraries(simplex2 PRIVATE fmt::fmt)

add_executable(simplex3 simplex3.cpp)
target_link_libraries(simplex3 PRIVATE ctre::ctre fmt::fmt)