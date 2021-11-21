
# for spew that is intolerable even when the logger is disabled
DEFINES+= DebugSafelyItself=1

CONFIG+=doitSafely
CONFIG+=usbSafely
CONFIG+=rpiSafely
include("../consoleapp.pro")


SOURCES += bonnet.cpp


