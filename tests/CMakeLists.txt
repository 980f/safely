cmake_minimum_required(VERSION 4.2)
project(tester)  #don't name a project the same as its directory?
set(safely_target tester)

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

add_safely(cppext)
add_safely(system)
add_safely(posix)
#caused infinite include of this file: add_safely(tests)  #first for project's safely.h
target_include_directories(${safely_target} PUBLIC ${PROJECT_SOURCE_DIR})
