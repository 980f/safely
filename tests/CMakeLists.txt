cmake_minimum_required(VERSION 4.2)
project(tester)  #don't name a project the same as its directory?
set (safely_target tester)

#suppress a warning about using a default value:
add_compile_definitions(SafelyApplicationEvents=5)

add_executable(${safely_target}
        main.cpp
        continuedFractionDemo.cpp
    filereadertester.cpp
  filewritertester.cpp
  testpathparser.cpp
  unicodetester.cpp
  jsontests.cpp
)

set(CMAKE_CXX_STANDARD 20)

function (add_safely safelyDir)
    add_subdirectory(${PROJECT_SOURCE_DIR}/../${safelyDir}/ ${CMAKE_CURRENT_BINARY_DIR}/${safelyDir})
endfunction()

add_safely(cppext)
add_safely(system)
add_safely(posix)
#caused infinite include of this file: add_safely(tests)  #first for project's safely.h
target_include_directories(${safely_target} PUBLIC ${PROJECT_SOURCE_DIR})

function(print_all_variables)
    message(STATUS "CMake Variables:")
    get_cmake_property(_variableNames VARIABLES)
    list(SORT _variableNames)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endfunction()

#print_all_variables()