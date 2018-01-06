#qtcreator project for testing 'safely' library
CONFIG += doitSafely
CONFIG += usbSafely
CONFIG += rpiSafely

prjdir=$$PWD
include(../consoleapp.pro)

SOURCES += \
    $$prjdir/sst.cpp

#HEADERS += \

# distfiles group doesn't matter to our build or deploy, but it is handy for rapid access to non-code files.
DISTFILES += \
    $$prjdir/sst.json
