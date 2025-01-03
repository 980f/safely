message("including usb with ftdi devices as well")

target_sources(${safely_target} PUBLIC
  mpsse.cpp
  mpsse.h
  usbid.cpp
  usbid.h
  libusber.cpp
  libusber.h
  ftdlib.cpp
  ftdlib.h
  #3rd party sources:
  ftdi.c
  ftdi.h
  ftdi_i.h
  ftdi_version_i.h
  libusb/core.c
  libusb/descriptor.c
  libusb/hotplug.c
  libusb/io.c
  libusb/strerror.c
  libusb/sync.c
  libusb/config.h
  libusb/hotplug.h
  libusb/libusb.h
  libusb/libusbi.h
  libusb/version.h
  libusb/version_nano.h
  libusb/os/linux_usbfs.h
  libusb/os/poll_posix.h
  libusb/os/threads_posix.h

  libusb/os/poll_posix.c
  libusb/os/threads_posix.c
  libusb/os/linux_usbfs.c
  libusb/os/linux_netlink.c
  libusb/os/linux_udev.c

)

target_include_directories(${safely_target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/libusb)

get_target_property(arf ${safely_target} "INCLUDE_DIRECTORIES")
message("usb includes: ${arf}")
