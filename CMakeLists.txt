cmake_minimum_required(VERSION 3.16)
project(staticlib VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(GNUInstallDirs)

include(FindPkgConfig)
#sigc 2.0 isn't convenient to use with cmake. Yet another nastiness that cmake brought to the world.
pkg_check_modules(DEPS REQUIRED sigc++-3.0)
include_directories(${DEPS_INCLUDE_DIRS})
#todo:00 target_link_libraries(yourprogram ${DEPS_LIBRARIES})

add_subdirectory(cppext)
add_subdirectory(system)
add_subdirectory(posix)
add_subdirectory(usb)
add_subdirectory(rasbpi)

add_library(safely STATIC
  ./ignoresignwarnings.h
  ./network/darkhttpd.cpp
  ./network/darkhttpd.h
  #sigc3 issues ./network/tcpserver.cpp
  ./network/tcpserver.h
  #sigc3 issues ./network/tcpsocket.cpp
  ./network/tcpsocket.h
  ./safely.h
  ./storedloggermanager.cpp
  ./storedloggermanager.h
  ./versioning.cpp
  ./versioning.h
)

add_executable(
  testlink
  tests/bonnet.cpp
)

target_link_libraries( testlink
  cppext
  system
  posix
  safely
)

include_directories(
 #useless: ${CMAKE_CURRENT_SOURCE_DIR}
  .
  cppext
  system
  posix
  network
  usb
  usb/libusb
  rasbpi
)
