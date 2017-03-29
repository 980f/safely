
## qtcreator .pro file fragment for this sibling library
PRE_TARGETDEPS += ../libposix.a
DEPENDPATH += ../posix

#includes this lib in your build
INCLUDEPATH += ../posix

#almost certainly redundant path to safely libs:
LIBS += -L..

#get posix linked many times:
LIBS += @../posix/posixgroup.ld

#for async IO (aio_*), as used in filereader:
LIBS += -lrt

#you will have to explicitly include cppext/lib.pro and (probably) system/lib.pro
