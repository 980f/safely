
# for spew that is intolerable even when the logger is disabled
DEFINES+= DebugSafelyItself=1

CONFIG+=doitSafely
CONFIG+=usbSafely
CONFIG+=rpiSafely
include("../consoleapp.pro")

#apparently xdo packager did not supply one of these: unix: PKGCONFIG += libxdo-dev

LIBS += -lxdo

SOURCES += bonnet.cpp



