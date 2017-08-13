include ("consoleapp.pro")

#build into the deployment source directory, presently the same as the project file's directory
TARGET = pialyzer
DESTDIR = $$PWD/5onpi/

#sigc stuff may be in the lib functions used.
include("sigc.pro")

#OS code
DEPENDPATH += posix
INCLUDEPATH += posix

SOURCES += \
    posix/fdset.cpp \
    posix/fildes.cpp \
    posix/posixwrapper.cpp \
    posix/perftimer.cpp \
    posix/stopwatch.cpp \
    posix/activityperformance.cpp \
    posix/filer.cpp \
    posix/posixlogger.cpp \
    posix/nanoseconds.cpp \
    posix/fileasynchio.cpp \
    posix/fileinfo.cpp \
    posix/filereader.cpp \
    posix/filewriter.cpp \
    posix/epoller.cpp \
    posix/application.cpp \
    posix/incrementalfiletransfer.cpp \
    posix/memorymapper.cpp \
    posix/timerfd.cpp \
    posix/microseconds.cpp \
    posix/telnetserver.cpp \
    posix/socketeer.cpp \
    posix/jsonfile.cpp \
    cppext/unionizer.cpp


HEADERS += \
    posix/timerfd.h \
    posix/fdset.h \
    posix/fildes.h \
    posix/filer.h \
    posix/posixwrapper.h \
    posix/perftimer.h \
    posix/stopwatch.h \
    posix/posixlogger.h \
    posix/nanoseconds.h \
    posix/fcntlflags.h \
    posix/fileasynchio.h \
    posix/fileinfo.h \
    posix/filereader.h \
    posix/filewriter.h \
    posix/epoller.h \
    posix/application.h \
    posix/incrementalfiletransfer.h \
    posix/memorymapper.h \
    posix/microseconds.h \ 
    cppext/unionizer.h


##get posix linked many times: names is relative to build directory, not project
#LIBS += @../posix/posixgroup.ld

#for async IO (aio_*), as used in filereader: rt:"glibc runtime"
LIBS += -lrt

#heaped code
DEPENDPATH += system
INCLUDEPATH += system
SOURCES += \
    system/logger.cpp \
    system/numberformatter.cpp \
    system/storable.cpp \
    system/storedipv4address.cpp \
    system/storednumeric.cpp \
    system/storedrange.cpp \
    system/textpointer.cpp \
    system/storejson.cpp \
    system/segmentedname.cpp \
    system/storedlabel.cpp \
    system/storedgroup.cpp \
    system/pathparser.cpp \
    system/sigcuser.cpp \
    system/gatedsignal.cpp \
    system/stored.cpp \
    system/dottedname.cpp \
    system/filename.cpp \
    system/filenameconverter.cpp \
    system/textformatter.cpp

HEADERS += \
    system/chain.h \
    system/chainsorter.h \
    system/logger.h \
    system/numberformatter.h \
    system/storable.h \
    system/storedgroup.h \
    system/storedipv4address.h \
    system/storednumeric.h \
    system/storedrange.h \
    system/textkey.h \
    system/textpointer.h \
    system/watchable.h \
    system/sigcuser.h \
    system/storejson.h \
    system/storedlabel.h \
    system/pathparser.h \
    system/stored.h \
    system/filename.h \
    system/dottedname.h \
    system/allocatedindexer.h


#heapless code
DEPENDPATH += cppext
INCLUDEPATH += cppext

DEFINES += LoggerManagement=0

SOURCES += \
    cppext/argset.cpp \
    cppext/charformatter.cpp \
    cppext/charscanner.cpp \
    cppext/cheaptricks.cpp \
    cppext/cycler.cpp \
    cppext/enumerizer.cpp \
    cppext/minimath.cpp \
    cppext/numberparser.cpp \
    cppext/platform.cpp \
    cppext/ranged.cpp \
    cppext/transactionalbuffer.cpp \
    cppext/utf8.cpp \
    cppext/textkey.cpp \
    cppext/bitwise.cpp \
    cppext/cstr.cpp \
    cppext/pushedjsonparser.cpp \
    cppext/halfopen.cpp \
    cppext/char.cpp \
    cppext/block.cpp \
    cppext/converter.cpp \
    cppext/abstractjsonparser.cpp \
    cppext/pushedparser.cpp \
    cppext/localonexit.cpp \
    cppext/bufferformatter.cpp \
    cppext/numberformat.cpp \
    cppext/countdown.cpp \
    cppext/numberpieces.cpp \
    cppext/stacked.cpp \
    cppext/polledtimer.cpp \
    cppext/softpwm.cpp \
    cppext/maxtracked.cpp \
    cppext/abstractpin.cpp


HEADERS += safely.h \
    cppext/buffer.h \
    cppext/chained.h \
    cppext/changemonitored.h \
    cppext/charformatter.h \
    cppext/charscanner.h \
    cppext/cheaptricks.h \
    cppext/cycler.h \
    cppext/enumerizer.h \
    cppext/extremer.h \
    cppext/eztypes.h \
    cppext/lock.h \
    cppext/minimath.h \
    cppext/multilock.h \
    cppext/numberparser.h \
    cppext/ordinator.h \
    cppext/sequence.h \
    cppext/transactionalbuffer.h \
    cppext/textkey.h \
    cppext/pathname.h \
    cppext/bitwise.h \
    cppext/segmentedname.h \
    cppext/cstr.h \
    cppext/pushedjsonparser.h \
    cppext/halfopen.h \
    cppext/char.h \
    cppext/index.h \
    cppext/block.h \
    cppext/converter.h \
    cppext/abstractjsonparser.h \
    cppext/pushedparser.h \
    cppext/localonexit.h \
    cppext/bufferformatter.h \
    cppext/numberformat.h \
    cppext/countdown.h \
    cppext/numberpieces.h \
    cppext/hook.h \
    cppext/stacked.h \
    cppext/onexit.h \
    cppext/polledtimer.h \
    cppext/softpwm.h \
    cppext/maxtracked.h \
    cppext/abstractpin.h


DEPENDPATH += usb
INCLUDEPATH += usb
SOURCES += \
    usb/ftdlib.cpp \
    usb/libusber.cpp \
    usb/ftdi.c \
    usb/usbid.cpp 

HEADERS += \
    usb/ftdi_i.h \
    usb/ftdi.h \
    usb/ftdlib.h \
    usb/libusber.h \
    usb/usbid.h 

#libftdi stuff needs this:
QMAKE_CFLAGS +=-std=c11

#originally I included the package, but it swallows some useful error messages so I copied the essential files into this project.
#CONFIG += link_pkgconfig
#PKGCONFIG += libusb-1.0

usblibdir = usb/libusb

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



#amptek (related) code
DEPENDPATH += 5onpi
INCLUDEPATH += 5onpi


SOURCES += \
    5onpi/main.cpp \
    5onpi/gpio.cpp \
    5onpi/amptekanalyzer.cpp \
    5onpi/serialdevice.cpp \
    5onpi/command.cpp \
    5onpi/minix.cpp \
    5onpi/dp5/paramset.cpp \
    5onpi/dp5/dp5.cpp \
    5onpi/analysis.cpp \
    5onpi/dp5/stats.cpp \
    5onpi/dp5/spectrum.cpp \
    5onpi/dp5/packet.cpp \
    5onpi/mpsse.cpp \
    5onpi/spectrumfile.cpp


HEADERS += \
    5onpi/gpio.h \
    5onpi/amptekanalyzer.h \
    5onpi/serialdevice.h \
    5onpi/command.h \
    5onpi/minix.h \
    5onpi/d2xx.h \
    5onpi/dp5/paramset.h \
    5onpi/dp5/dp5.h \
    5onpi/dp5/dppvariant.h \
    5onpi/analysis.h \
    5onpi/dp5/stats.h \
    5onpi/dp5/spectrum.h \
    5onpi/dp5/packet.h \
    5onpi/mpsse.h \
    5onpi/spectrumfile.h

# distfiles doesn't matter to our build or deploy, but it is handy for rapid access to non-code files.
DISTFILES += \
    5onpi/pialyzer.json \
    5onpi/mycraft3.json \
    5onpi/andyh-T420.json \
    5onpi/farmer.json

