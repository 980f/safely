
# for spew that is intolerable even when the logger is disabled
DEFINES+= DebugSafelyItself=1

CONFIG+=doitSafely
CONFIG+=usbSafely
CONFIG+=rpiSafely
CONFIG+=networkSafely #using bonnet as testbed for new module
include("../consoleapp.pro")


SOURCES += bonnet.cpp


