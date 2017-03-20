include("../consoleapp.pro")

#linaro configuration target:
target.path = /home/pi/work/ # path on device
INSTALLS += target
#end raspberry pi target

#actually using sigc stuff locally as well as hidden perhaps in the libs.
include("../sigc.pro")
#our sibling with OS code
include("../posix/lib.pro")
#our sibling with heaped code
include("../system/lib.pro")
#our sibling with heapless code
include("../cppext/lib.pro")


INCLUDEPATH += settings

SOURCES += main.cpp \
    jsontests.cpp \
    unicodetester.cpp \
    testabstractjsonparser.cpp \
    testpathparser.cpp \
    posix.cpp \
    systemtester.cpp \
    settings/bidisetting.cpp \
    settings/markreport.cpp \
    settings/pcucontrol.cpp \
    settings/positionersettings.cpp \
    settings/settingstestdatum.cpp \
    settings/steppercontrol.cpp \
    settings/artcore.cpp \
    settings/artprotocolprocessor.cpp \
    settings/doorlatchsetting.cpp \
    settings/lampsettings.cpp \
    settings/purgerconfig.cpp \
    settings/asciiframer.cpp \
    settings/fifo.cpp \
    filereadertester.cpp \
    testhooker.cpp \
    filewritertester.cpp

HEADERS += \
    unicodetester.h \
    testabstractjsonparser.h \
    testpathparser.h \
    systemtester.h \
    settings/bidisetting.h \
    settings/markreport.h \
    settings/pcucontrol.h \
    settings/positionersettings.h \
    settings/settingstestdatum.h \
    settings/steppercontrol.h \
    settings/artcore.h \
    settings/artprotocolprocessor.h \
    settings/doorlatchsetting.h \
    settings/lampsettings.h \
    settings/purgerconfig.h \
    settings/art.h \
    settings/asciiframer.h \
    settings/asciiframing.h \
    settings/fifo.h \
    settings/serialagent.h \
    filereadertester.h \
    testhooker.h \
    filewritertester.h

DISTFILES += \
    filereadertester.0 \
    filereadertester.1
