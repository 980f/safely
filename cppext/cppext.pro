# // *INDENT-OFF* protects against pretty printers
#-------------------------------------------------
#
# Project created by QtCreator 2016-12-14T15:17:49
#
#-------------------------------------------------

QT       -= core gui

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

TARGET = cppext

#permalloc needs a define: this is only non-zero in microcontrollers (heapless systems)
DEFINES += PermAllocPool=0

INCLUDEPATH += ..

SOURCES += cppexttester.cpp \
    argset.cpp \
    bigendianer.cpp \
    centeredslice.cpp \
    charformatter.cpp \
    charscanner.cpp \
    cheapTricks.cpp \
    core-atomic.cpp \
    crc16m.cpp \
    cycler.cpp \
    determinator.cpp \
    enumerated.cpp \
    enumerizer.cpp \
    fitstat.cpp \
    interval.cpp \
    linear.cpp \
    linearfit.cpp \
    linearmapping.cpp \
    minimath.cpp \
    multilock.cpp \
    numberparser.cpp \
    permalloc.cpp \
    platform.cpp \
    polynomial.cpp \
    quadraticfilter.cpp \
    quantizer.cpp \
    ranged.cpp \
    realstatistic.cpp \
    roundrobiner.cpp \
    safestr.cpp \
    settable.cpp \
    transactionalbuffer.cpp \
    twiddler.cpp \
    utf8.cpp \
    textkey.cpp \
    polyfilter.cpp \
    zstring.cpp

HEADERS += cppexttester.h \
    safely.h \
    argset.h \
    bigendianer.h \
    buffer.h \
    centeredslice.h \
    chained.h \
    changemonitored.h \
    charformatter.h \
    charscanner.h \
    cheapTricks.h \
    circularbuffer.h \
    circularindexer.h \
    core-atomic.h \
    crc16m.h \
    cycler.h \
    debuggable.h \
    determinator.h \
    enumerated.h \
    enumerizer.h \
    eventFlag.h \
    extremer.h \
    eztypes.h \
    fitstat.h \
    interval.h \
    iterate.h \
    linear.h \
    linearfit.h \
    linearmapping.h \
    linearsmoother.h \
    linearstatistic.h \
    lock.h \
    minimath.h \
    multilock.h \
    numberparser.h \
    ordinator.h \
    permalloc.h \
    polynomial.h \
    quadraticfilter.h \
    quantizer.h \
    range.h \
    ranged.h \
    realstatistic.h \
    roundrobiner.h \
    safestr.h \
    scaler.h \
    sequence.h \
    settable.h \
    syncretic.h \
    transactionalbuffer.h \
    twiddler.h \
    utf8.h \
    textkey.h \
    polyfilter.h \
    zstring.h

