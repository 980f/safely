# // *INDENT-OFF*  in case we run uncrustify on this flie

#magic for making a static lib
include ("../staticlib.pro")
#our sibling with heapless code
include("../cppext/lib.pro")
#our sibling with heapless code
include("../system/lib.pro")


INCLUDEPATH += ..

SOURCES += \
    fdset.cpp \
    fildes.cpp \
    posixwrapper.cpp \
    filer.cpp

HEADERS += \
    fdset.h \
    fildes.h \
    filer.h \
    posixwrapper.h \
    vargs.h
