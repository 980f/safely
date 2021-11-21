==== BASE ====
## QT project file fragment with things common to libraries  and applications using the 'safely' project
safelydir=$$PWD

CONFIG -= app_bundle
CONFIG -= qt
#we want the same language in use everywhere:
CONFIG += c++14
#rpi gcc is now 6.x, the following may be redundant
QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS += -funsigned-char

#signed chars are such a pain.
QMAKE_CFLAGS += -funsigned-char
QMAKE_CXXFLAGS += -funsigned-char
#perhaps: QMAKE_CFLAGS += -Wno-pointer-sign

#safely projects put their application specific library altering includes above the module src path.
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

#add doitSafely to CONFIG to get this chunk of including most of the safely libraries
doitSafely {
  include("$$safelydir/sigc.pro")
  include("$$safelydir/cppext/cppext.pro")
  include("$$safelydir/system/system.pro")
  include("$$safelydir/posix/posix.pro")

  #project specific code
  DEPENDPATH += $$prjdir
  INCLUDEPATH += $$prjdir
}

#add usbSafely to CONFIG to get the usb host library
usbSafely {
  include("$$safelydir/usb/usblibs.pro")
}

#add rpiSafely to CONFIG to get the Raspberry Pi application library, with stuff like direct access to GPIO pins.
rpiSafely {
  include("$$safelydir/rasbpi/rasbpi.pro")
}

#any headers or source declarations following this line are a user-error via qtcreator, move to appropriate pro file.
