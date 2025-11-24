cmake_minimum_required (VERSION 3.21)
include(FetchContent)
project (aoc_2025)

function(dump_cmake_variables)
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        if (ARGV0)
            unset(MATCHED)

            #case sensitive match
            # string(REGEX MATCH ${ARGV0} MATCHED ${_variableName})
            #
            #case insenstitive match
            string( TOLOWER "${ARGV0}" ARGV0_lower )
            string( TOLOWER "${_variableName}" _variableName_lower )
            string(REGEX MATCH ${ARGV0_lower} MATCHED ${_variableName_lower})

            if (NOT MATCHED)
                continue()
            endif()
        endif()
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endfunction()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# dump_cmake_variables(CXX)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    add_compile_options(-fexperimental-library -mmacos-version-min=15.6)
#    add_link_options(-Wl -macos_version_min=15.6)
    add_link_options(-Wl)
endif()

FetchContent_Declare( ctre 
    GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions
    GIT_TAG origin/main
    )
FetchContent_MakeAvailable(ctre)
# uncomment when/if necessary!
#find_package(Boost 1.88 REQUIRED)
find_package(z3 QUIET)
if(z3_FOUND)
    add_compile_definitions(USE_Z3)
    message(STATUS "Got Z3")
endif()
find_package(mdspan CONFIG REQUIRED)
find_path(SYSTEM_INCLUDE_DIR experimental/mdspan)
include_directories(${SYSTEM_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/common)
find_package(fmt CONFIG REQUIRED)

add_subdirectory(aoc1)
add_subdirectory(aoc2)
add_subdirectory(aoc3)
add_subdirectory(aoc4)
add_subdirectory(aoc5)
add_subdirectory(aoc6)
add_subdirectory(aoc7)
add_subdirectory(aoc8)
add_subdirectory(aoc9)
add_subdirectory(aoc10)
add_subdirectory(aoc11)
add_subdirectory(aoc12)
