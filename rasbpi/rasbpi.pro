##raspberry pi components

pidir = $$PWD
message("Including raspberry pi hardware parts from $$pidir")

DEPENDPATH += $$pidir
INCLUDEPATH += $$pidir
SOURCES += \
    $$pidir/gpio.cpp \
    $$pidir/dout.cpp \
    $$pidir/din.cpp \
    $$pidir/inputmonitor.cpp \
    $$pidir/pwm.cpp \
    $$pidir/clockgenerator.cpp \
    $$pidir/peripheral.cpp \
    $$pidir/i2c.cpp


HEADERS += \
    $$pidir/gpio.h \
    $$pidir/dout.h \
    $$pidir/din.h \
    $$pidir/inputmonitor.h \
    $$pidir/pwm.h \
    $$pidir/clockgenerator.h \
    $$pidir/peripheral.h \
    $$pidir/i2c.h
