cmake_minimum_required(VERSION 3.8)
project(sst)

set(CMAKE_CXX_STANDARD 11)

#it took 15 minutes to sift through the web to figure out this:
include(FindPkgConfig)
pkg_check_modules(SIGC sigc++-2.0)
#find_package(sigc++-2.0)  #didn't do anythign discernable

# another 30 minutes giving up on package management, manually discovered via qmake:
#include_directories(/usr/include/sigc++-2.0 /usr/lib/x86_64-linux-gnu/sigc++-2.0/include)

include_directories(${SIGC_INCLUDE_DIRS} )
link_directories(${SIGC_LIBRARY_DIRS} )
#target_link_libraries(${SIGC_LIBRARIES} )

#safely has a habit of wandering around:
set(safely ..)

set(cppext ${safely}/cppext)
set(system ${safely}/system)
set(posix ${safely}/posix)

include_directories(${safely})

include_directories(${cppext})
set(CPPEXT_FILES
  ${cppext}/enumerated.cpp
  ${cppext}/textkey.cpp
  ${cppext}/minimath.cpp
  )

include_directories(${system})
set(SYSTEM_FILES
  ${system}/storable.cpp
  ${system}/stored.cpp
  ${system}/filenameconverter.cpp
  )

include_directories(${posix})
set(POSIX_FILES
  ${posix}/posixlogger.cpp
  )


set(SOURCE_FILES
  ${CPPEXT_FILES}
  ${SYSTEM_FILES}
  ${POSIX_FILES}
  sst.cpp)

add_executable(sst ${SOURCE_FILES})