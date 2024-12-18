#include(thisfile) to get access to sigc support
#invoking this allows us to use pkg_check_* functions to get at libraries that have been installed by the local system package manager. The Package in FIND_PACKAGE is cmake package, not system package.
FIND_PACKAGE(PkgConfig)

if(${PKG_CONFIG_FOUND})
else()
MESSAGE("package config support is missing on this platform")
ENDIF()

pkg_check_modules(SIGC sigc++-2.0)
if(${SIGC_FOUND})
#MESSAGE(SIGC found: )
#MESSAGE(INC: ${SIGC_INCLUDE_DIRS})
#MESSAGE(-L: ${SIGC_LIBRARY_DIRS})
#MESSAGE(linklibs: ${SIGC_LINK_LIBRARIES})
#MESSAGE(ldflags: ${SIGC_LDFLAGS})
#MESSAGE(ldflagsmore: ${SIGC_LDFLAGS_OTHER})
#MESSAGE(cflags: ${SIGC_CFLAGS})
#MESSAGE(cflagsmore: ${SIGC_CFLAGS_OTHER})
#MESSAGE(libs: ${SIGC_LIBRARIES})
#the following are necessary, but cmake will cache them after which you can remove these and still build, while some other user will get errors. BAD CMAKE!
  INCLUDE_DIRECTORIES(${SIGC_INCLUDE_DIRS})
  LINK_LIBRARIES(sigc-2.0)
ELSE()
  MESSAGE("sigc system package not found, sigc not usable")
ENDIF()
