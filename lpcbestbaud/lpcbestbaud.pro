include("../consoleapp.pro")
include("../cppext/lib.pro")

TARGET = lpcbestbaud

SOURCES += \
    lpcbaud.cpp \
    ../../cortexm/lpc/baudsearch.cpp

HEADERS += \
    ../../cortexm/lpc/baudsearch.h
