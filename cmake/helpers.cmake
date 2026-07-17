#cmake utilities

# this helper presumes that your project is under safely, probably should excise the safely tester where this was first used and make it an independent git repo.
function(add_safely safelyDir)
  add_subdirectory(${PROJECT_SOURCE_DIR}/../${safelyDir}/ ${CMAKE_CURRENT_BINARY_DIR}/${safelyDir})
endfunction()

#debug all settings
function(print_all_variables)
  message(STATUS "CMake Variables:")
  get_cmake_property(_variableNames VARIABLES)
  list(SORT _variableNames)
  foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
  endforeach ()
endfunction()

