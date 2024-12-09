cmake_minimum_required(VERSION 3.16)
project(staticlib VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(GNUInstallDirs)

include(FindPkgConfig)
#sigc 2.0 isn't convenient to use with cmake. Yet another nastiness that cmake brought to the world.
pkg_check_modules(DEPS REQUIRED sigc++-3.0)
include_directories(${DEPS_INCLUDE_DIRS})
#todo:00 target_link_libraries(yourprogram ${DEPS_LIBRARIES})

include_directories(
  cppext
  system
  posix
  network
  usb
  rasbpi
)

add_library(safely STATIC
  ./cppext/poissonfeeder.cpp
  ./cppext/crc16m.cpp
  ./cppext/linearfit.cpp
  ./cppext/quantizer.cpp
  ./cppext/realstatistic.cpp
  ./cppext/safestr.cpp
  ./cppext/core-atomic.cpp
  ./cppext/bitwise.cpp
  ./cppext/bitbanger.cpp
  ./cppext/maxtracked.cpp
  ./cppext/pathname.cpp
  ./cppext/bigendianer.cpp
  ./cppext/converter.cpp
  ./cppext/countdown.cpp
  ./cppext/enumerizer.cpp
  ./cppext/halfopen.cpp
  ./cppext/localonexit.cpp
  ./cppext/multilock.cpp
  ./cppext/pushedparser.cpp
  ./cppext/textchunk.cpp
  ./cppext/textwalker.cpp
  ./cppext/transactionalbuffer.cpp
  ./cppext/unionizer.cpp
  ./cppext/cheaptricks.cpp
  ./cppext/cycler.cpp

  ./cppext/polledtimer.cpp
  ./cppext/ranged.cpp
  ./cppext/softpwm.cpp
  ./cppext/twiddler.cpp
  ./cppext/bufferformatter.cpp
  ./cppext/centeredslice.cpp
  ./cppext/char.cpp
  ./cppext/charformatter.cpp
  ./cppext/charscanner.cpp
  ./cppext/continuedfractionratiogenerator.cpp
  ./cppext/cstr.cpp
  ./cppext/enumerated.cpp
  ./cppext/fitstat.cpp
  ./cppext/numberformat.cpp
  ./cppext/pushedjsonparser.cpp
  ./cppext/quadraticfilter.cpp
  ./cppext/textkey.cpp
  ./cppext/utf8.cpp
  ./cppext/pathiterator.cpp
  ./cppext/minimath.cpp
  ./cppext/bitlock.cpp
  ./cppext/determinator.cpp
  ./cppext/polynomial.cpp
  ./cppext/numberparser.cpp
  ./cppext/numberpieces.cpp
  ./cppext/numericalvalue.cpp
  ./cppext/polyfilter.cpp
  ./system/urltext.cpp
 # ./system/cachedindextostored.cpp
  ./system/storedcorrelation.cpp
  ./system/utf8transcoder.cpp
  ./system/peakfindrecords.cpp
  ./system/delimitedinputstream.cpp
  ./system/runoncenthtime.cpp
  ./system/filenameconverter.cpp
  ./system/linearfilter.cpp
  ./system/allocatedindexer.cpp
  ./system/delimitingoutputstream.cpp
  ./system/numberformatter.cpp
  ./system/sigcuser.cpp
  ./system/storageexporter.cpp

  ./system/storedgroup.cpp
  ./system/storednumeric.cpp
  ./system/storedrange.cpp
  ./system/logger.cpp
  ./system/managedblock.cpp
  ./system/dottedname.cpp
  ./system/dyndeterminator.cpp
  ./system/filename.cpp
  ./system/gatedsignal.cpp
  ./system/jsonstore.cpp
  #needs ArtFile ./system/kanjidic.cpp
  #formatter hierarchy seems to have evaporated ./system/measure.cpp
  ./system/pathparser.cpp
  ./system/segmentedname.cpp
  ./system/stored.cpp
  ./system/storedenum.cpp
  ./system/storedipv4address.cpp
  ./system/storedlabel.cpp
  ./system/storejson.cpp
  ./system/textformatter.cpp
  ./system/textpointer.cpp
  ./system/treefile.cpp
  ./system/utf8text.cpp
  ./system/matrixinverter.cpp
  ./system/storable.cpp
  ./system/utf8converter.cpp
  #when did this functionality disappear? what was it? ./posix/vargs.cpp
  ./posix/activityperformance.cpp
  ./posix/directory.cpp
  #gutted, had alternate timerfd wtf?! ./posix/eventloop.cpp
  ./posix/iosource.cpp
  ./posix/performancetimer.cpp
  ./posix/streamformatter.cpp
  ./posix/streamprintf.cpp
  ./posix/tcpserver.cpp
  ./posix/tcptester.cpp
  ./posix/fileinfo.cpp
  ./posix/filereader.cpp
  ./posix/jasoned.cpp
  ./posix/jsonfiletracker.cpp
  ./posix/memorymapper.cpp
  ./posix/performancelogger.cpp
  ./posix/serveroptions.cpp
  ./posix/socketeer.cpp
  ./posix/directorywalker.cpp
  ./posix/epoller.cpp
  ./posix/fileasynchio.cpp
  ./posix/filer.cpp
  ./posix/filewriter.cpp
  ./posix/microseconds.cpp
  ./posix/perftimer.cpp
  ./posix/stopwatch.cpp
  ./posix/tcpsocket.cpp
  ./posix/telnetserver.cpp
  ./posix/threader.cpp
  ./posix/timerfd.cpp
  ./posix/worstcase.cpp
  ./posix/serialdevice.cpp
  ./posix/application.cpp
  ./posix/posixwrapper.cpp
  ./posix/jsonfile.cpp
  ./posix/fdset.cpp
  ./posix/incrementalfiletransfer.cpp
  ./posix/fildes.cpp
  ./posix/filewatcher.cpp
  ./posix/nanoseconds.cpp
  ./posix/posixlogger.cpp
  ./ustring.cpp
  ./versioning.cpp
  ./usb/ftdlib.cpp
  ./usb/mpsse.cpp
  ./usb/usbid.cpp
  ./usb/libusber.cpp
  ./storedloggermanager.cpp
  ./rasbpi/clockgenerator.cpp
  ./rasbpi/din.cpp
  ./rasbpi/dout.cpp
  ./rasbpi/inputmonitor.cpp
  ./rasbpi/peripheral.cpp
  ./rasbpi/piinfo.cpp
  ./rasbpi/gpio.cpp
  ./rasbpi/SSD1306.cpp
  ./rasbpi/i2c.cpp
  ./rasbpi/pwm.cpp
  ./network/tcpserver.cpp
  ./network/tcpsocket.cpp

  ./ignoresignwarnings.h
  ./cppext/core-atomic.h
  ./cppext/circularindexer.h
  ./cppext/crc16m.h
  ./cppext/debuggable.h
  ./cppext/linearfit.h
  ./cppext/linearstatistic.h
  ./cppext/scaler.h
  ./cppext/cppexttester.h
  ./cppext/pathname.h
  ./cppext/realstatistic.h
  ./cppext/abstractpin.h
  ./cppext/converter.h
  ./cppext/countdown.h
  ./cppext/enumerizer.h
  ./cppext/halfopen.h
  ./cppext/lock.h
  ./cppext/maxtracked.h
  ./cppext/multilock.h
  ./cppext/numericalvalue.h
  ./cppext/pushedparser.h
  ./cppext/quantizer.h
  ./cppext/textchunk.h
  ./cppext/textwalker.h
  ./cppext/transactionalbuffer.h
  ./cppext/unionizer.h
  ./cppext/bigendianer.h
  ./cppext/bitwise.h
  ./cppext/changemonitored.h
  ./cppext/char.h
  ./cppext/onexit.h
  ./cppext/poissonfeeder.h
  ./cppext/polledtimer.h
  ./cppext/pushedjsonparser.h
  ./cppext/range.h
  ./cppext/ranged.h
  ./cppext/softpwm.h
  ./cppext/syncretic.h
  ./cppext/tableofpointers.h
  ./cppext/abstractjsonparser.h
  ./cppext/bitbanger.h
  ./system/block.h
  ./cppext/bufferformatter.h
  ./cppext/centeredslice.h
  ./cppext/chained.h
  ./cppext/charformatter.h
  ./cppext/charscanner.h
  ./cppext/circularbuffer.h
  ./cppext/continuedfractionratiogenerator.h
  ./cppext/cstr.h
  ./cppext/cycler.h
  ./cppext/enumerated.h
  ./cppext/eventflag.h
  ./cppext/eztypes.h
  ./cppext/fitstat.h
  ./cppext/hook.h
  ./cppext/index.h
  ./cppext/numberformat.h
  ./cppext/numberparser.h
  ./cppext/ordinator.h
  ./cppext/polynomial.h
  ./cppext/quadraticfilter.h
  ./cppext/safestr.h
  ./cppext/sequence.h
  ./cppext/twiddler.h
  ./cppext/utf8.h
  ./cppext/buffer.h
  ./cppext/demonic.h
  ./cppext/pathiterator.h
  ./cppext/textkey.h
  ./cppext/iterate.h
  ./cppext/extremer.h
  ./cppext/cheaptricks.h
  ./cppext/minimath.h
  ./cppext/bitlock.h
  ./cppext/determinator.h
  ./cppext/numberpieces.h
  ./cppext/polyfilter.h
  ./cppext/linearsmoother.h
  #./system/kanjidic.h
  #./system/cachedindextostored.h
  ./system/delimitedinputstream.h
  ./system/measure.h
  ./system/treefile.h
  ./system/peakfindrecords.h
  ./system/mapiterator.h
  ./system/utf8transcoder.h
  ./system/utf8text.h
  ./system/linearfilter.h
  ./system/storedcorrelation.h
  ./system/storageexporter.h
  ./system/runoncenthtime.h
  ./system/dottedname.h
  ./system/pathparser.h
  ./system/storedipv4address.h
  ./system/watchable.h
  ./system/delimitingoutputstream.h
  ./system/numberformatter.h
  ./system/sigcuser.h

  ./system/urltext.h
  ./system/storedrange.h
  ./system/filename.h
  ./system/managedblock.h
  ./system/storejson.h
  ./system/allocatedindexer.h
  ./system/chainsorter.h
  ./system/dyndeterminator.h
  ./system/filenameconverter.h
  ./system/gatedsignal.h
  ./system/jsonstore.h
  ./system/matrixinverter.h
  ./system/segmentedname.h
  ./system/storedenum.h
  ./system/storednumeric.h
  ./system/storedsettable.h
  ./system/textformatter.h
  ./system/textpointer.h
  ./system/utf8converter.h
  ./system/logger.h
  ./system/chain.h
  ./system/storable.h
  ./system/stored.h
  ./system/storedgroup.h
  ./system/storedlabel.h
  ./posix/activityperformance.h
  ./posix/eventloop.h
  ./posix/streamformatter.h
  ./posix/streamprintf.h
  ./posix/tcpserver.h
  ./posix/tcptester.h
  ./posix/fileasynchio.h
  ./posix/fileinfo.h
  ./posix/jasoned.h
  ./posix/jsonfiletracker.h
  ./posix/memorymapper.h
  ./posix/performancelogger.h
  ./posix/performancetimer.h
  ./posix/serveroptions.h
  ./posix/socketeer.h
  ./posix/telnetserver.h
  ./posix/directory.h
  ./posix/directorywalker.h
  ./posix/epoller.h
  ./posix/fcntlflags.h
  ./posix/filer.h
  ./posix/filewriter.h
  ./posix/microseconds.h
  ./posix/nanoseconds.h
  ./posix/perftimer.h
  ./posix/serialdevice.h
  ./posix/stopwatch.h
  ./posix/tcpsocket.h
  ./posix/threader.h
  ./posix/timerfd.h
  ./posix/worstcase.h
  ./posix/application.h
  ./posix/jsonfile.h
  ./posix/fdset.h
  ./posix/fildes.h
  ./posix/filereader.h
  ./posix/incrementalfiletransfer.h
  ./posix/iosource.h
  ./posix/posixwrapper.h
  ./posix/filewatcher.h
  ./posix/posixlogger.h
  ./usb/ftdi_i.h
  ./usb/ftdi_version_i.h
  ./usb/libusb/config.h
  ./usb/libusb/hotplug.h
  ./usb/libusb/libusb.h
  ./usb/libusb/libusbi.h
  ./usb/libusb/os/poll_posix.h
  ./usb/libusb/os/threads_posix.h
  ./usb/libusb/os/linux_usbfs.h
  ./usb/libusb/version.h
  ./usb/libusb/version_nano.h
  ./usb/mpsse.h
  ./usb/usbid.h
  ./usb/ftdlib.h
  ./usb/libusber.h
  ./usb/ftdi.h
  ./rasbpi/clockgenerator.h
  ./rasbpi/dout.h
  ./rasbpi/inputmonitor.h
  ./rasbpi/peripheral.h
  ./rasbpi/piinfo.h
  ./rasbpi/pwm.h
  ./rasbpi/gpio.h
  ./rasbpi/SSD1306.h
  ./rasbpi/i2c.h
  ./rasbpi/din.h
  ./ustring.h
  ./storedloggermanager.h
  ./network/tcpsocket.h
  ./network/tcpserver.h
  ./safely.h
)
