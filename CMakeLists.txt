cmake_minimum_required(VERSION 3.16)
project(staticlib VERSION 1.0 LANGUAGES C CXX)

add_executable(
  testcppext
  tests/main.cpp
)
set(safely_target testcppext)

#below here should be generic enough to be a separate cmake file that other projects include
include(GNUInstallDirs)

include(FindPkgConfig)
pkg_check_modules(DEPS REQUIRED sigc++-3.0)
include_directories(${DEPS_INCLUDE_DIRS})

target_link_libraries(${safely_target} ${DEPS_LIBRARIES})

target_link_libraries( ${safely_target}
  sigc-3.0    #sigc+-3.0 did not provide a cmake file to deal with them not naming their lib for their package.
  udev        #system lib that used to be found automatically but now needs our help.
)
#we include pretty much everything to every project, to avert the tedium of carefully listing those that each target needs. Since all the targets in this file are for library testing that is a reasonable thing to do.
include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})  #safely itself
add_subdirectory(cppext)
add_subdirectory(system)
add_subdirectory(posix)
add_subdirectory(network)
add_subdirectory(usb)
add_subdirectory(rasbpi)


get_target_property(arf ${safely_target} "INCLUDE_DIRECTORIES")
message("aggregate includes: ${arf}")
