
include("../consoleapp.pro")

#our sibling with heaped code
include("../system/lib.pro")
#our sibling with heapless code
include("../cppext/lib.pro")
#our sibling with OS code
include("../posix/lib.pro")


SOURCES += main.cpp \
    ../posix/streamprintf.cpp

HEADERS += \
    ../posix/streamprintf.h
