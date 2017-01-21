include("../consoleapp.pro")

#actually using sigc stuff locally as well as hidden perhaps in the libs.
include("../sigc.pro")
#our sibling with heaped code
include("../system/lib.pro")
#our sibling with heapless code
include("../cppext/lib.pro")
#our sibling with OS code
include("../posix/lib.pro")


SOURCES += main.cpp \
    jsontests.cpp \
    unicodetester.cpp \
    testabstractjsonparser.cpp \
    testpathparser.cpp \
    positionersettings.cpp \
    markreport.cpp \
    steppercontrol.cpp \
    pcucontrol.cpp

HEADERS += \
    unicodetester.h \
    testabstractjsonparser.h \
    testpathparser.h \
    positionersettings.h \
    markreport.h \
    steppercontrol.h \
    pcucontrol.h
