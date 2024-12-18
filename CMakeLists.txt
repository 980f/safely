cmake_minimum_required(VERSION 3.16)
project(staticlib VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(GNUInstallDirs)

include(FindPkgConfig)
#sigc 2.0 isn't convenient to use with cmake. Yet another nastiness that cmake brought to the world.
pkg_check_modules(DEPS REQUIRED sigc++-3.0)
include_directories(${DEPS_INCLUDE_DIRS})
#todo:00 target_link_libraries(yourprogram ${DEPS_LIBRARIES})

add_subdirectory(cppext)
add_subdirectory(system)

add_library(safely STATIC
  ./ignoresignwarnings.h
  ./network/darkhttpd.cpp
  ./network/darkhttpd.h
  #sigc3 issues ./network/tcpserver.cpp
  ./network/tcpserver.h
  #sigc3 issues ./network/tcpsocket.cpp
  ./network/tcpsocket.h
  ./rasbpi/clockgenerator.cpp
  ./rasbpi/clockgenerator.h
  ./rasbpi/din.cpp
  ./rasbpi/din.h
  ./rasbpi/dout.cpp
  ./rasbpi/dout.h
  ./rasbpi/gpio.cpp
  ./rasbpi/gpio.h
  ./rasbpi/i2c.cpp
  ./rasbpi/i2c.h
  ./rasbpi/inputmonitor.cpp
  ./rasbpi/inputmonitor.h
  ./rasbpi/peripheral.cpp
  ./rasbpi/peripheral.h
  ./rasbpi/piinfo.cpp
  ./rasbpi/piinfo.h
  ./rasbpi/pwm.cpp
  ./rasbpi/pwm.h
  ./rasbpi/SSD1306.cpp
  ./rasbpi/SSD1306.h
  ./safely.h
  ./storedloggermanager.cpp
  ./storedloggermanager.h

  ./usb/ftdi.h
  ./usb/ftdi_i.h
  ./usb/ftdi_version_i.h
  ./usb/ftdlib.cpp
  ./usb/ftdlib.h
  ./usb/libusb/config.h
  ./usb/libusber.cpp
  ./usb/libusber.h
  ./usb/libusb/hotplug.h
  ./usb/libusb/libusb.h
  ./usb/libusb/libusbi.h
  ./usb/libusb/os/linux_usbfs.h
  ./usb/libusb/os/poll_posix.h
  ./usb/libusb/os/threads_posix.h
  ./usb/libusb/version.h
  ./usb/libusb/version_nano.h
  ./usb/mpsse.cpp
  ./usb/mpsse.h
  ./usb/usbid.cpp
  ./usb/usbid.h
  ./ustring.cpp
  ./ustring.h
  ./versioning.cpp

)

add_executable(
  testlink
  tests/bonnet.cpp

)

target_link_libraries( testlink
  cppext
  system
  posix
  safely
)

include_directories(
  #  cppext
  system
  posix
  network
  usb
  usb/libusb
  rasbpi
)
