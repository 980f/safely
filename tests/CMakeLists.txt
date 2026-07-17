cmake_minimum_required(VERSION 4.2)
project(tests)
set (safely_target tests)

#suppress a warning about using a default value:
add_compile_definitions(SafelyApplicationEvents=5)

add_executable(${safely_target}
        main.cpp
        continuedFractionDemo.cpp)

set(CMAKE_CXX_STANDARD 20)

function (add_safely safelyDir)
    add_subdirectory(${PROJECT_SOURCE_DIR}/../${safelyDir}/ ${CMAKE_CURRENT_BINARY_DIR}/${safelyDir})
endfunction()

add_safely(cppext)
add_safely(system)
add_safely(posix)

function(print_all_variables)
    message(STATUS "CMake Variables:")
    get_cmake_property(_variableNames VARIABLES)
    list(SORT _variableNames)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endfunction()

#print_all_variables()