# // *INDENT-OFF* protects against pretty printers
#magic for making a static lib

include ("../staticlib.pro")

#permalloc needs a define: this is only non-zero in microcontrollers (heapless systems)
DEFINES += PermAllocPool=0

SOURCES += argset.cpp \
    bigendianer.cpp \
    centeredslice.cpp \
    charformatter.cpp \
    charscanner.cpp \
    cheaptricks.cpp \
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
    hassettings.cpp \
    pathname.cpp \
    bitwise.cpp \
    cstr.cpp \
    pushedjsonparser.cpp \
    halfopen.cpp \
    char.cpp \
    block.cpp \
    converter.cpp \
    abstractjsonparser.cpp

HEADERS += safely.h \
    argset.h \
    bigendianer.h \
    buffer.h \
    centeredslice.h \
    chained.h \
    changemonitored.h \
    charformatter.h \
    charscanner.h \
    cheaptricks.h \
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
    zstring.h \
    hassettings.h \
    pathname.h \
    bitwise.h \
    segmentedname.h \
    cstr.h \
    testcppextbuild.h \
    pushedjsonparser.h \
    halfopen.h \
    char.h \
    index.h \
    block.h \
    converter.h \
    abstractjsonparser.h
