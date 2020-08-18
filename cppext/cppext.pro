##qtcreator project file for cpp stuff that doesn't use dynamic memory
# it doesn't cover all files in the directory, add them as you validate them.

cppextdir=$$PWD
message("Including heapless parts from $$cppextdir")


DEPENDPATH += $$cppextdir
INCLUDEPATH += $$cppextdir


SOURCES += \
    $$cppextdir/argset.cpp \
    $$cppextdir/charformatter.cpp \
    $$cppextdir/charscanner.cpp \
    $$cppextdir/cheaptricks.cpp \
    $$cppextdir/cycler.cpp \
    $$cppextdir/enumerizer.cpp \
    $$cppextdir/minimath.cpp \
    $$cppextdir/numberparser.cpp \
    $$cppextdir/platform.cpp \
    $$cppextdir/ranged.cpp \
    $$cppextdir/transactionalbuffer.cpp \
    $$cppextdir/utf8.cpp \
    $$cppextdir/textkey.cpp \
    $$cppextdir/bitwise.cpp \
    $$cppextdir/cstr.cpp \
    $$cppextdir/pushedjsonparser.cpp \
    $$cppextdir/halfopen.cpp \
    $$cppextdir/char.cpp \
    $$cppextdir/block.cpp \
    $$cppextdir/converter.cpp \
    $$cppextdir/abstractjsonparser.cpp \
    $$cppextdir/pushedparser.cpp \
    $$cppextdir/localonexit.cpp \
    $$cppextdir/bufferformatter.cpp \
    $$cppextdir/numberformat.cpp \
    $$cppextdir/countdown.cpp \
    $$cppextdir/numberpieces.cpp \
    $$cppextdir/stacked.cpp \
    $$cppextdir/polledtimer.cpp \
    $$cppextdir/softpwm.cpp \
    $$cppextdir/maxtracked.cpp \
    $$cppextdir/abstractpin.cpp \
    $$cppextdir/unionizer.cpp \
    $$cppextdir/numericalvalue.cpp \
    $$cppextdir/roundrobiner.cpp \
    $$cppextdir/settable.cpp \
    $$cppextdir/textwalker.cpp \
    $$cppextdir/textchunk.cpp \
    $$cppextdir/continuedfractionratiogenerator.cpp

HEADERS += safely.h \
    $$cppextdir/buffer.h \
    $$cppextdir/chained.h \
    $$cppextdir/changemonitored.h \
    $$cppextdir/charformatter.h \
    $$cppextdir/charscanner.h \
    $$cppextdir/cheaptricks.h \
    $$cppextdir/cycler.h \
    $$cppextdir/enumerizer.h \
    $$cppextdir/extremer.h \
    $$cppextdir/eztypes.h \
    $$cppextdir/lock.h \
    $$cppextdir/minimath.h \
    $$cppextdir/multilock.h \
    $$cppextdir/numberparser.h \
    $$cppextdir/ordinator.h \
    $$cppextdir/sequence.h \
    $$cppextdir/transactionalbuffer.h \
    $$cppextdir/textkey.h \
    $$cppextdir/pathname.h \
    $$cppextdir/bitwise.h \
    $$cppextdir/segmentedname.h \
    $$cppextdir/cstr.h \
    $$cppextdir/pushedjsonparser.h \
    $$cppextdir/halfopen.h \
    $$cppextdir/char.h \
    $$cppextdir/index.h \
    $$cppextdir/block.h \
    $$cppextdir/converter.h \
    $$cppextdir/abstractjsonparser.h \
    $$cppextdir/pushedparser.h \
    $$cppextdir/localonexit.h \
    $$cppextdir/bufferformatter.h \
    $$cppextdir/numberformat.h \
    $$cppextdir/countdown.h \
    $$cppextdir/numberpieces.h \
    $$cppextdir/hook.h \
    $$cppextdir/stacked.h \
    $$cppextdir/onexit.h \
    $$cppextdir/polledtimer.h \
    $$cppextdir/softpwm.h \
    $$cppextdir/maxtracked.h \
    $$cppextdir/abstractpin.h\
    $$cppextdir/unionizer.h \
    $$cppextdir/numericalvalue.h \
    $$cppextdir/textwalker.cpp \
    $$cppextdir/textchunk.h \
    $$cppextdir/continuedfractionratiogenerator.h \
    $$cppextdir/demonic.h


