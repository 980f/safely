include ("../consoleapp.pro")

#sigc stuff may be in the lib functions used.
include("../sigc.pro")
#OS code
#include("../posix/posix.files")
include("../posix/lib.pro")

#heaped code
#include("../system/system.files")
include("../system/lib.pro")

#heapless code
#include("../cppext/cppext.files")
include("../cppext/lib.pro")

#cheat to get posix linked many times: create a softlink liblogger.a that points to it and then ...
LIBS += @../posix/posixgroup.ld
#-lposix2

#set the following for to 1 a binary for real hardware
DEFINES += FOR_LIVE_PI=0
#//can't get environment stable accross my systems $$(IAMPI)

SOURCES += \
    main.cpp \
    gpio.cpp \
    consoleapplication.cpp \
    serialdevice.cpp \
    dp5device.cpp \
    command.cpp

HEADERS += \
    gpio.h \
    piinfo.h \
    consoleapplication.h \
    serialdevice.h \
    dp5device.h \
    command.h

DISTFILES += \
    mycraft3.json \
    andy-T420.json
