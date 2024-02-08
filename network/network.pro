## qtcreator project file for things that need a posix system.

include("../sigc.pro")

networkdir=$$PWD

INCLUDEPATH+=$$networkdir
DEPENDPATH+=$$networkdir

message("Including (inter)network parts from $$networkdir")

SOURCES += \
    $$networkdir/tcpsocket.cpp \
    $$networkdir/tcpserver.cpp

HEADERS += \
    $$networkdir/tcpsocket.h \
    $$networkdir/tcpserver.h

#for async IO (aio_*)
LIBS += -lrt


