cmake_minimum_required(VERSION 3.16)
project(staticlib VERSION 1.0 LANGUAGES C CXX)

add_executable(
  testlink
  tests/bonnet.cpp
  cppext/bundler.h
)
include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set(safely_target testlink)
#PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
#below here should be generic enough to be a separate cmake file that other projects include

include(GNUInstallDirs)

include(FindPkgConfig)

pkg_check_modules(DEPS REQUIRED sigc++-3.0)
include_directories(${DEPS_INCLUDE_DIRS})

target_link_libraries(${safely_target} ${DEPS_LIBRARIES})

target_link_libraries( ${safely_target}
  sigc-3.0
  udev
)

add_subdirectory(cppext)
add_subdirectory(system)
add_subdirectory(posix)
add_subdirectory(network)
add_subdirectory(usb)
add_subdirectory(rasbpi)


get_target_property(arf testlink "INCLUDE_DIRECTORIES")
message("aggregate includes: ${arf}")
