
##magic for making a static lib
include ("../staticlib.pro")

DEFINES += LoggerManagement=0

include ("cppext.files")

HEADERS += \
    maxtracked.h

SOURCES += \
    maxtracked.cpp

