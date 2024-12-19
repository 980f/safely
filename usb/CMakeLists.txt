  message("including usb with ftdi devices as well")

add_library(usb STATIC
  mpsse.cpp
  mpsse.h
  usbid.cpp
  usbid.h
  libusber.cpp
  libusber.h
  ftdlib.cpp
  ftdlib.h
  #3rd party sources:
  ftdi.h
  ftdi_i.h
  ftdi_version_i.h
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


  target_include_directories(usb PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/cppext
    ${PROJECT_SOURCE_DIR}/system
    ${PROJECT_SOURCE_DIR}/posix
    ${CMAKE_CURRENT_SOURCE_DIR}
  )

  get_target_property(arf usb "INCLUDE_DIRECTORIES")
  message("'usb' includes ${arf}")
