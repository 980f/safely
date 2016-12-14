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

INCLUDEPATH += ../cppext

SOURCES += systemtester.cpp \
    cachedindextostored.cpp \
    cpshelpers.cpp \
    delimitedinputstream.cpp \
    delimitingoutputstream.cpp \
    dyndeterminator.cpp \
    jsonstore.cpp \
    kanjidic.cpp \
    linearfilter.cpp \
    logger.cpp \
    matrixinverter.cpp \
    measure.cpp \
    networkscanner.cpp \
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
    textkey.cpp \
    textpointer.cpp \
    tokenstream.cpp \
    treefile.cpp \
    peakfindrecords.cpp

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
    smartptr.h \
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
    sigcuser.h

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += sigc++-2.0
