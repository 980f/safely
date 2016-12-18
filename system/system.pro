# // *INDENT-OFF*  in case we run uncrustify on this flie
#-------------------------------------------------
#
# Project created by QtCreator 2016-12-14T16:27:12
#
#-------------------------------------------------

TARGET = system

QT       -= core gui

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../ ../cppext

SOURCES += systemtester.cpp \
    cachedindextostored.cpp \
    cpshelpers.cpp \
    delimitedinputstream.cpp \
    delimitingoutputstream.cpp \
    dyndeterminator.cpp \
    linearfilter.cpp \
    logger.cpp \
    matrixinverter.cpp \
    measure.cpp \
    numberformatter.cpp \
    perftimer.cpp \
    stopwatch.cpp \
    storable.cpp \
    storageexporter.cpp \
    storedcorrelation.cpp \
    storedenum.cpp \
    storedipv4address.cpp \
    storednumeric.cpp \
    storedrange.cpp \
    storedsettable.cpp \
    textpointer.cpp \
    treefile.cpp \
    peakfindrecords.cpp \
    gatedsignal.cpp

HEADERS += systemtester.h \
    cachedindextostored.h \
    chain.h \
    chainsorter.h \
    cpshelpers.h \
    delimitedinputstream.h \
    delimitingoutputstream.h \
    dyndeterminator.h \
    jsonstore.h \
    kanjidic.h \
    linearfilter.h \
    logger.h \
    mapiterator.h \
    matrixinverter.h \
    measure.h \
    networkscanner.h \
    numberformatter.h \
    perftimer.h \
    stopwatch.h \
    storable.h \
    storageexporter.h \
    storedcorrelation.h \
    storedenum.h \
    storedgroup.h \
    storedipv4address.h \
    storednumeric.h \
    storedrange.h \
    storedsettable.h \
    textkey.h \
    textpointer.h \
    tokenstream.h \
    treefile.h \
    watchable.h \
    peakfindrecords.h \
    sigcuser.h \
    gatedsignal.h

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += sigc++-2.0
# the PKGCONFIG line above should have taken care of the following: //todo: remove explicit includepath and debug pkgconfig for sigc.
INCLUDEPATH += /usr/include/sigc++-2.0
