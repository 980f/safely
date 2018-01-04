##qtcreator project file for include libusb components

usbdir=$$PWD

message("Including usb parts from $$usbdir")

DEPENDPATH += $$usbdir
INCLUDEPATH += $$usbdir

SOURCES += \
    $$usbdir/libusber.cpp \
    $$usbdir/usbid.cpp

HEADERS += \
    $$usbdir/libusber.h \
    $$usbdir/usbid.h


#originally I included the package, but it swallows some useful error messages so I copied the essential files into this project.
#CONFIG += link_pkgconfig
#PKGCONFIG += libusb-1.0

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

#//no HEADERS here as the ones needed are all in the same directory as the .c files.

#usb lib uses this so's
LIBS += -lpthread -ludev

