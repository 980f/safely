
# for spew that is intolerable even when the logger is disabled
DEFINES+= DebugSafelyItself=1

CONFIG+=doitSafely
CONFIG+=usbSafely
CONFIG+=rpiSafely
include("../consoleapp.pro")


SOURCES += main.cpp \
    jsontests.cpp \
    unicodetester.cpp \
    testabstractjsonparser.cpp \
    testpathparser.cpp \
    posix.cpp \
    systemtester.cpp \
    filereadertester.cpp \
    testhooker.cpp \
    filewritertester.cpp \
    backgroundfilereader.cpp \
    ../system/utf8text.cpp \
    ../system/utf8converter.cpp \
    ../system/utf8transcoder.cpp

HEADERS += \
    unicodetester.h \
    testabstractjsonparser.h \
    testpathparser.h \
    systemtester.h \
    filereadertester.h \
    testhooker.h \
    filewritertester.h \
    backgroundfilereader.h \
    ../system/utf8text.h \
    ../system/utf8converter.h \
    ../system/utf8transcoder.h

DISTFILES += \
    filereadertester.0 \
    filereadertester.1 \
    ../qt-prettyprinter.py
