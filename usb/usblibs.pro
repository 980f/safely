
#originally I included the package, but it swallows some useful error messages so I copied the essential files into this project.
#CONFIG += link_pkgconfig
#PKGCONFIG += libusb-1.0

#include this for libusb as local source
usblibdir = ../usb/libusb

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


