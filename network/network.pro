## qtcreator project file for things that need a posix system.

include("../sigc.pro")

networkdir=$$PWD

INCLUDEPATH+=$$networkdir
DEPENDPATH+=$$networkdir

message("Including (inter)network parts from $$networkdir")

SOURCES += \
    $$networkdir/fdset.cpp \
    $$networkdir/fildes.cpp \
    $$networkdir/posixwrapper.cpp \
    $$networkdir/perftimer.cpp \
    $$networkdir/stopwatch.cpp \
    $$networkdir/activityperformance.cpp \
    $$networkdir/filer.cpp \
    $$networkdir/posixlogger.cpp \
    $$networkdir/nanoseconds.cpp \
    $$networkdir/fileasynchio.cpp \
    $$networkdir/fileinfo.cpp \
    $$networkdir/filereader.cpp \
    $$networkdir/filewriter.cpp \
    $$networkdir/epoller.cpp \
    $$networkdir/application.cpp \
    $$networkdir/incrementalfiletransfer.cpp \
    $$networkdir/memorymapper.cpp \
    $$networkdir/timerfd.cpp \
    $$networkdir/microseconds.cpp \
    $$networkdir/telnetserver.cpp \
    $$networkdir/socketeer.cpp \
    $$networkdir/jsonfile.cpp \
    $$networkdir/serialdevice.cpp \
    $$networkdir/serveroptions.cpp \
    $$networkdir/jasoned.cpp \
    $$networkdir/filewatcher.cpp \
    $$networkdir/worstcase.cpp \
    $$networkdir/threader.cpp \
    $$networkdir/directorywalker.cpp

HEADERS += \
    $$networkdir/timerfd.h \
    $$networkdir/fdset.h \
    $$networkdir/fildes.h \
    $$networkdir/serialdevice.h \
    $$networkdir/filer.h \
    $$networkdir/posixwrapper.h \
    $$networkdir/perftimer.h \
    $$networkdir/stopwatch.h \
    $$networkdir/posixlogger.h \
    $$networkdir/nanoseconds.h \
    $$networkdir/fcntlflags.h \
    $$networkdir/fileasynchio.h \
    $$networkdir/fileinfo.h \
    $$networkdir/filereader.h \
    $$networkdir/filewriter.h \
    $$networkdir/epoller.h \
    $$networkdir/application.h \
    $$networkdir/incrementalfiletransfer.h \
    $$networkdir/memorymapper.h \
    $$networkdir/microseconds.h \
    $$networkdir/serveroptions.h \
    $$networkdir/jasoned.h \
    $$networkdir/filewatcher.h \
    $$networkdir/worstcase.h \
    $$networkdir/threader.h \
    $$networkdir/directorywalker.h

#for async IO (aio_*), as used in filereader: rt:"glibc runtime"
LIBS += -lrt


