# // *INDENT-OFF*  in case we run uncrustify on this flie

#magic for making a static lib
include ("../staticlib.pro")
#our sibling with heapless code
include("../cppext/lib.pro")
#we'll use system sigc code
include("../sigc.pro")

SOURCES += \
    delimitedinputstream.cpp \
    delimitingoutputstream.cpp \
    dyndeterminator.cpp \
    linearfilter.cpp \
    logger.cpp \
    matrixinverter.cpp \
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
    peakfindrecords.cpp \
    gatedsignal.cpp \
    storejson.cpp \
    segmentedname.cpp \
    utf8text.cpp \
    storedlabel.cpp \
    storedgroup.cpp \
    pathparser.cpp \
    textformatter.cpp \
    sigcuser.cpp \
    runoncenthtime.cpp \
    stored.cpp \
    filename.cpp

HEADERS += \
    cachedindextostored.h \
    chain.h \
    chainsorter.h \
    cpshelpers.h \
    delimitedinputstream.h \
    delimitingoutputstream.h \
    dyndeterminator.h \
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
    gatedsignal.h \
    storejson.h \
    utf8text.h \
    storedlabel.h \
    pathparser.h \
    textformatter.h \
    runoncenthtime.h \
    stored.h \
    filename.h

