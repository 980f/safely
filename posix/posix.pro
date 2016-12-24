# // *INDENT-OFF*  in case we run uncrustify on this flie

#magic for making a static lib
include ("../staticlib.pro")
#our sibling with heapless code
include("../cppext/lib.pro")

INCLUDEPATH += ..

SOURCES += \
    fdset.cpp \
    fildes.cpp \
    posixwrapper.cpp

HEADERS += \
    fdset.h \
    fildes.h \
    filer.h \
    posixwrapper.h \
    vargs.h
