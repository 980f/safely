## QT project file fragment with things common to libraries  and applications using the 'safely' project
safelydir=$$PWD

CONFIG -= app_bundle
CONFIG -= qt
#we want the same language in use everywhere:
CONFIG += c++14
#rpi gcc is 4.9, needs a kick in the std:
QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS += -funsigned-char

#signed chars are such a pain.
QMAKE_CFLAGS += -funsigned-char
#perhaps: QMAKE_CFLAGS += -Wno-pointer-sign

#safely projects put their application specific library altering includes above the module src path.
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

doitSafely {
  include("$$safelydir/sigc.pro")
  include("$$safelydir/cppext/cppext.pro")
  include("$$safelydir/system/system.pro")
  include("$$safelydir/posix/posix.pro")

  #project specific code
  DEPENDPATH += $$prjdir
  INCLUDEPATH += $$prjdir
}

usbSafely {
  include("$$safelydir/usb/usblibs.pro")
}

rpiSafely {
  include("$$safelydir/rasbpi/rasbpi.pro")
}

#any headers or source declarations following this line are a user-error via qtcreator, move to appropriate pro file.
