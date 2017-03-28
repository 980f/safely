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

#cheat to get posix linked twice: create a softlink liblogger.a that points to it and then ...
LIBS += -llogger

#set the following for to 1 a binary for real hardware
IAMPI=0
DEFINES += FOR_LIVE_PI=$$IAMPI
#//can't get environment stable accross my systems $$(IAMPI)

message("IAMPI is " $$(IAMPI))

SOURCES += \
    main.cpp \
    gpio.cpp \
    platform.cpp \
    consoleapplication.cpp \
    finddevice.cpp \
    serialdevice.cpp \
    dp5device.cpp \
    command.cpp

HEADERS += \
    gpio.h \
    piinfo.h \
    consoleapplication.h \
    finddevice.h \
    serialdevice.h \
    dp5device.h \
    command.h

DISTFILES += \
    options.json
