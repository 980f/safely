## qtcreator project file for things that need a posix system.

##todo: confirm sigc is used directly herein
#include("../sigc.pro")

posixdir=$$PWD

INCLUDEPATH+=$$posixdir
DEPENDPATH+=$$posixdir

message("Including posix parts from $$posixdir")

SOURCES += \
    $$posixdir/fdset.cpp \
    $$posixdir/fildes.cpp \
    $$posixdir/posixwrapper.cpp \
    $$posixdir/perftimer.cpp \
    $$posixdir/stopwatch.cpp \
    $$posixdir/activityperformance.cpp \
    $$posixdir/filer.cpp \
    $$posixdir/posixlogger.cpp \
    $$posixdir/nanoseconds.cpp \
    $$posixdir/fileasynchio.cpp \
    $$posixdir/fileinfo.cpp \
    $$posixdir/filereader.cpp \
    $$posixdir/filewriter.cpp \
    $$posixdir/epoller.cpp \
    $$posixdir/application.cpp \
    $$posixdir/incrementalfiletransfer.cpp \
    $$posixdir/memorymapper.cpp \
    $$posixdir/timerfd.cpp \
    $$posixdir/microseconds.cpp \
    $$posixdir/telnetserver.cpp \
    $$posixdir/socketeer.cpp \
    $$posixdir/jsonfile.cpp \
    $$posixdir/serialdevice.cpp \
    $$posixdir/serveroptions.cpp \
    $$posixdir/jasoned.cpp \
    $$posixdir/filewatcher.cpp \
    $$posixdir/worstcase.cpp

HEADERS += \
    $$posixdir/timerfd.h \
    $$posixdir/fdset.h \
    $$posixdir/fildes.h \
    $$posixdir/serialdevice.h \
    $$posixdir/filer.h \
    $$posixdir/posixwrapper.h \
    $$posixdir/perftimer.h \
    $$posixdir/stopwatch.h \
    $$posixdir/posixlogger.h \
    $$posixdir/nanoseconds.h \
    $$posixdir/fcntlflags.h \
    $$posixdir/fileasynchio.h \
    $$posixdir/fileinfo.h \
    $$posixdir/filereader.h \
    $$posixdir/filewriter.h \
    $$posixdir/epoller.h \
    $$posixdir/application.h \
    $$posixdir/incrementalfiletransfer.h \
    $$posixdir/memorymapper.h \
    $$posixdir/microseconds.h \
    $$posixdir/serveroptions.h \
    $$posixdir/jasoned.h \
    $$posixdir/filewatcher.h \
    $$posixdir/worstcase.h


#for async IO (aio_*), as used in filereader: rt:"glibc runtime"
LIBS += -lrt


