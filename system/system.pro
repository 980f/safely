##qtcreator project file for functions that need heap

systemdir=$$PWD

message("Including heap dependent parts from $$systemdir")

DEPENDPATH += $$systemdir
INCLUDEPATH += $$systemdir

SOURCES += \
    $$systemdir/logger.cpp \
    $$systemdir/numberformatter.cpp \
    $$systemdir/storable.cpp \
    $$systemdir/storedipv4address.cpp \
    $$systemdir/storednumeric.cpp \
    $$systemdir/storedrange.cpp \
    $$systemdir/textpointer.cpp \
    $$systemdir/storejson.cpp \
    $$systemdir/segmentedname.cpp \
    $$systemdir/storedlabel.cpp \
    $$systemdir/storedgroup.cpp \
    $$systemdir/pathparser.cpp \
    $$systemdir/sigcuser.cpp \
    $$systemdir/gatedsignal.cpp \
    $$systemdir/stored.cpp \
    $$systemdir/dottedname.cpp \
    $$systemdir/filename.cpp \
    $$systemdir/filenameconverter.cpp \
    $$systemdir/textformatter.cpp  \
    $$systemdir/storedargs.cpp

HEADERS += \
    $$systemdir/chain.h \
    $$systemdir/chainsorter.h \
    $$systemdir/logger.h \
    $$systemdir/numberformatter.h \
    $$systemdir/storable.h \
    $$systemdir/storedgroup.h \
    $$systemdir/storedipv4address.h \
    $$systemdir/storednumeric.h \
    $$systemdir/storedrange.h \
    $$systemdir/textkey.h \
    $$systemdir/textpointer.h \
    $$systemdir/watchable.h \
    $$systemdir/sigcuser.h \
    $$systemdir/storejson.h \
    $$systemdir/storedlabel.h \
    $$systemdir/pathparser.h \
    $$systemdir/stored.h \
    $$systemdir/filename.h \
    $$systemdir/dottedname.h \
    $$systemdir/allocatedindexer.h \
    $$systemdir/storedargs.h


