#sigc3.cmake

#it takes 4 lines of cmake to use a system library.
find_package(PkgConfig REQUIRED)
pkg_check_modules(SIGC REQUIRED sigc++-3.0)

#expose to all targets, if that becomes odious then we have to use target_ versions
INCLUDE_DIRECTORIES(${SIGC_INCLUDE_DIRS})
LINK_LIBRARIES(${SIGC_LIBRARIES})
  
#target_link_libraries(${PROJECT_NAME} ${SIGC_LIBRARIES})
#target_include_directories(${PROJECT_NAME} PRIVATE ${SIGC_INCLUDE_DIRS})
#end sigc included block

