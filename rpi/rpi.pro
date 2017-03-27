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

#uncommment the following for running on real hardware
DEFINES += FOR_LIVE_PI=0
##didn't work, got a blank $$(IAMPI)

message("FOR_LIVE_PI is $$(FOR_LIVE_PI)")

SOURCES += \
    main.cpp \
    gpio.cpp

HEADERS += \
    gpio.h \
    piinfo.h
