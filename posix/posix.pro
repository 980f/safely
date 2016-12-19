## // *INDENT-OFF* in case we hit the file with uncrustify

TEMPLATE = lib
CONFIG += staticlib
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

INCLUDEPATH += .. ../cppext
LIBS += -L.. -lcppext

SOURCES += \
    fdset.cpp \
    fildes.cpp \
    posixwrapper.cpp \
    vargs.cpp

HEADERS += \
    fdset.h \
    fildes.h \
    filer.h \
    posixwrapper.h \
    vargs.h
