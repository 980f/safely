# // *INDENT-OFF*  in case we run uncrustify on this flie

#magic for making a static lib
include ("../staticlib.pro")
#our sibling with heapless code
include("../cppext/lib.pro")
#our sibling with mallocating code, but no OS per se:
include("../system/lib.pro")

#sigc in filer/jsonfile
include("../sigc.pro")


##for async IO add to your executable project:
#LIBS += -lrt

#include("posix.files")
#I put the above back in this file as qtcreator wouldn't text search them without some finesse

DEPENDPATH += ../posix
INCLUDEPATH += ../posix

#for async IO (aio_*), as used in filereader:
LIBS += -lrt


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
    nanoseconds.cpp \
    fileasynchio.cpp \
    fileinfo.cpp \
    filereader.cpp \
    filewriter.cpp \
    epoller.cpp \
    application.cpp \
    incrementalfiletransfer.cpp \
    memorymapper.cpp \
    directory.cpp \
    timerfd.cpp \
    microseconds.cpp \
    threader.cpp \
    jsonfile.cpp \
    performancetimer.cpp \
    activityperformance.cpp



HEADERS += \
    timerfd.h \
    fdset.h \
    fildes.h \
    filer.h \
    posixwrapper.h \
    perftimer.h \
    stopwatch.h \
    posixlogger.h \
    streamprintf.h \
    streamformatter.h \
    nanoseconds.h \
    fcntlflags.h \
    fileasynchio.h \
    fileinfo.h \
    filereader.h \
    filewriter.h \
    epoller.h \
    application.h \
    incrementalfiletransfer.h \
    memorymapper.h \
    directory.h \
    filenameconverter.h \
    microseconds.h \
    threader.h \
    jsonfile.h \
    performancetimer.h \
    activityperformance.h

DISTFILES += \
    posixgroup.ld

