#project specific relative paths
#directory for generally useful code, relative to this file:
safelydir=../safely
#directory for libusb and friends, relative to this file:
usbdir=$$safelydir/usb
#directory of project specific code, relative to this file:
prjdir=.

#build into the deployment source directory, presently the same as the project file's directory
TARGET = pialyzer
DESTDIR = $$PWD

#qt settings for a commandline application:
include ("$$safelydir/consoleapp.pro")

#sigc stuff may be in the lib functions used.
include("$$safelydir/sigc.pro")

#OS code
posixdir = $$safelydir/posix
DEPENDPATH += $$posixdir
INCLUDEPATH += $$posixdir

SOURCES += \
    $$posixdir/fdset.cpp \
    $$posixdir/fildes.cpp \
    $$posixdir/posixwrapper.cpp \
    $$posixdir/perftimer.cpp \
    $$posixdir/stopwatch.cpp \
    $$posixdir/activityperformance.cpp \
    $$posixdir/filer.cpp \
    $$posixdir/posixlogger.cpp \
    $$posixdir/nanoseconds.cpp \
    $$posixdir/fileasynchio.cpp \
    $$posixdir/fileinfo.cpp \
    $$posixdir/filereader.cpp \
    $$posixdir/filewriter.cpp \
    $$posixdir/epoller.cpp \
    $$posixdir/application.cpp \
    $$posixdir/incrementalfiletransfer.cpp \
    $$posixdir/memorymapper.cpp \
    $$posixdir/timerfd.cpp \
    $$posixdir/microseconds.cpp \
    $$posixdir/telnetserver.cpp \
    $$posixdir/socketeer.cpp \
    $$posixdir/jsonfile.cpp \
    $$posixdir/serialdevice.cpp \
    $$posixdir/filewatcher.cpp \
    $$posixdir/jsonfiletracker.cpp

HEADERS += \
    $$posixdir/timerfd.h \
    $$posixdir/fdset.h \
    $$posixdir/fildes.h \
    $$posixdir/serialdevice.h \
    $$posixdir/filer.h \
    $$posixdir/posixwrapper.h \
    $$posixdir/perftimer.h \
    $$posixdir/stopwatch.h \
    $$posixdir/posixlogger.h \
    $$posixdir/nanoseconds.h \
    $$posixdir/fcntlflags.h \
    $$posixdir/fileasynchio.h \
    $$posixdir/fileinfo.h \
    $$posixdir/filereader.h \
    $$posixdir/filewriter.h \
    $$posixdir/epoller.h \
    $$posixdir/application.h \
    $$posixdir/incrementalfiletransfer.h \
    $$posixdir/memorymapper.h \
    $$posixdir/microseconds.h \
    $$posixdir/filewatcher.h \
    $$posixdir/jsonfiletracker.h

#for async IO (aio_*), as used in filereader: rt:"glibc runtime"
LIBS += -lrt

#heaped code, stuff that relies upon malloc directly or indirectly
systemdir=$$safelydir/system

DEPENDPATH += $$systemdir
INCLUDEPATH += $$systemdir

#logger managment means a central utility to control all logging streams, which entails making a list of them as they are constructed.
DEFINES += LoggerManagement=0

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
    $$systemdir/textformatter.cpp

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
    $$systemdir/allocatedindexer.h


#heapless code
cppextdir=$$safelydir/cppext
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
    $$cppextdir/numericalvalue.cpp


#safely.h location is always ambiguous
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
    $$cppextdir/numericalvalue.cpp


pidir = $$safelydir/rasbpi
DEPENDPATH += $$pidir
INCLUDEPATH += $$pidir
SOURCES += \
    $$pidir/gpio.cpp \
    $$pidir/dout.cpp

HEADERS += \
    $$pidir/gpio.h \
    $$pidir/dout.h

#linux usb user-space access and common devices

DEPENDPATH += $$usbdir
INCLUDEPATH += $$usbdir
SOURCES += \
    $$usbdir/ftdlib.cpp \
    $$usbdir/libusber.cpp \
    $$usbdir/ftdi.c \
    $$usbdir/mpsse.cpp \
    $$usbdir/usbid.cpp

HEADERS += \
    $$usbdir/ftdi_i.h \
    $$usbdir/ftdi.h \
    $$usbdir/mpsse.cpp \
    $$usbdir/ftdlib.h \
    $$usbdir/libusber.h \
    $$usbdir/usbid.h

#libftdi stuff needs this:
QMAKE_CFLAGS +=-std=c11

usblibdir = $$usbdir/libusb

DEPENDPATH += $$usblibdir
INCLUDEPATH += $$usblibdir

SOURCES += \
  $$usblibdir/core.c \
  $$usblibdir/descriptor.c  \
  $$usblibdir/hotplug.c  \
  $$usblibdir/io.c  \
  $$usblibdir/strerror.c  \
  $$usblibdir/sync.c  \
  $$usblibdir/os/poll_posix.c  \
  $$usblibdir/os/threads_posix.c  \
  $$usblibdir/os/linux_usbfs.c  \
  $$usblibdir/os/linux_netlink.c  \
  $$usblibdir/os/linux_udev.c

LIBS += -lpthread -ludev


#project specific code
DEPENDPATH += $$prjdir
INCLUDEPATH += $$prjdir

SOURCES += \
    $$prjdir/main.cpp \
    $$prjdir/amptekanalyzer.cpp \
    $$prjdir/commander.cpp \
    $$prjdir/analysis.cpp \
    $$prjdir/command.cpp \
    $$prjdir/minix.cpp \
    $$prjdir/dp5/paramset.cpp \
    $$prjdir/dp5/dp5.cpp \
    $$prjdir/dp5/stats.cpp \
    $$prjdir/dp5/spectrum.cpp \
    $$prjdir/dp5/packet.cpp \
    $$prjdir/dp5/spectrumfile.cpp

HEADERS += \
    $$prjdir/amptekanalyzer.h \
    $$prjdir/commander.h \
    $$prjdir/analysis.h \
    $$prjdir/command.h \
    $$prjdir/minix.h \
    $$prjdir/dp5/paramset.h \
    $$prjdir/dp5/dp5.h \
    $$prjdir/dp5/dppvariant.h \
    $$prjdir/dp5/stats.h \
    $$prjdir/dp5/spectrum.h \
    $$prjdir/dp5/packet.h \
    $$prjdir/dp5/spectrumfile.h

# distfiles group doesn't matter to our build or deploy, but it is handy for rapid access to non-code files.
DISTFILES += \
    $$prjdir/pialyzer.json \
    $$prjdir/mycraft3.json \
    $$prjdir/andyh-T420.json \
    $$prjdir/farmer.json

