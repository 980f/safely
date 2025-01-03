cmake_minimum_required(VERSION 3.16)
project(staticlib VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(GNUInstallDirs)

include(FindPkgConfig)
#sigc 2.0 isn't convenient to use with cmake. Yet another nastiness that cmake brought to the world.
pkg_check_modules(DEPS REQUIRED sigc++-3.0)
include_directories(${DEPS_INCLUDE_DIRS})
#todo:00 target_link_libraries(yourprogram ${DEPS_LIBRARIES})

#add_library(safely STATIC
#  ./ignoresignwarnings.h
#  ./network/darkhttpd.cpp
#  ./network/darkhttpd.h
#  #sigc3 issues ./network/tcpserver.cpp
#  ./network/tcpserver.h
#  #sigc3 issues ./network/tcpsocket.cpp
#  ./network/tcpsocket.h
#  ./safely.h
#  ./storedloggermanager.cpp
#  ./storedloggermanager.h
#  ./versioning.cpp
#  ./versioning.h
#  system/textpointer.cpp
#  cppext/charformatter.cpp
#  cppext/cstr.cpp
#)

add_executable(
  testlink
  tests/bonnet.cpp
  cppext/bundler.h
)

set(safely_target testlink)

target_link_libraries( testlink
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

#include_directories(
# #useless: ${CMAKE_CURRENT_SOURCE_DIR}
#  .
#  cppext
#  system
#  posix
#  network
#  usb
#  usb/libusb
#  rasbpi
#)
