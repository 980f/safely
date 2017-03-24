include ("../consoleapp.pro")

#sigc stuff may be in the lib functions used.
include("../sigc.pro")
#OS code
include("../posix/posix.files")

#heaped code
include("../system/system.files")

#heapless code
include("../cppext/cppext.files")

#uncommment the following for running on real hardware
DEFINES += FOR_LIVE_PI=1

SOURCES += \
    main.cpp \
    gpio.cpp

HEADERS += \
    gpio.h \
    piinfo.h
