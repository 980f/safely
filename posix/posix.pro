# // *INDENT-OFF*  in case we run uncrustify on this flie

#magic for making a static lib
include ("../staticlib.pro")
#our sibling with heapless code
include("../cppext/lib.pro")
#our sibling with mallocating code, but no OS per se:
include("../system/lib.pro")

##for async IO add to your executable project:
#LIBS += -L/usr/lib/x86_64-linux-gnu  -lrt -lpthread

SOURCES += \
    fdset.cpp \
    fildes.cpp \
    posixwrapper.cpp \
    perftimer.cpp \
    stopwatch.cpp \
    filer.cpp \
    posixlogger.cpp \
    streamprintf.cpp \
    streamformatter.cpp \
    eventloop.cpp \
    nanoseconds.cpp
    fileasynchio.cpp \
    fileinfo.cpp \
    filereader.cpp

HEADERS += \
    fdset.h \
    fildes.h \
    filer.h \
    posixwrapper.h \
    perftimer.h \
    stopwatch.h \
    posixlogger.h \
    streamprintf.h \
    streamformatter.h \
    eventloop.h \
    nanoseconds.h
    fcntlflags.h \
    fileasynchio.h \
    fileinfo.h \
    filereader.h
