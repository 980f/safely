message("including posix library")

target_sources(${safely_target} PUBLIC
  activityperformance.cpp
  activityperformance.h
  application.cpp
  application.h
  directory.cpp
  directory.h
  directorywalker.cpp
  directorywalker.h
  epoller.cpp
  epoller.h
  #gutted, had alternate timerfd wtf?! eventloop.cpp
  eventloop.h
  fcntlflags.h
  fdset.cpp
  fdset.h
  fildes.cpp
  fildes.h
  fileasynchio.cpp
  fileasynchio.h
  fileinfo.cpp
  fileinfo.h
  filer.cpp
  filereader.cpp
  filereader.h
  filer.h
  #fix later:
  #filewatcher.cpp
  #filewatcher.h
  filewriter.cpp
  filewriter.h
  incrementalfiletransfer.cpp
  incrementalfiletransfer.h
  #iosource stuff  is far from working. there is so much legacy of glib confusing necessary functionality.
  #iosource.cpp
  #iosource.h
  jasoned.cpp
  jasoned.h
  jsonfile.cpp
  jsonfile.h
  #jsonfiletracker.cpp
  #jsonfiletracker.h
  memorymapper.cpp
  memorymapper.h
  microseconds.cpp
  microseconds.h
  nanoseconds.cpp
  nanoseconds.h
  performancelogger.cpp
  performancelogger.h
  performancetimer.cpp
  performancetimer.h
  perftimer.cpp
  perftimer.h
  posixlogger.cpp
  posixlogger.h
  posixwrapper.cpp
  posixwrapper.h
  serialdevice.cpp
  serialdevice.h
  stopwatch.cpp
  stopwatch.h
  streamformatter.cpp
  streamformatter.h
  streamprintf.cpp
  streamprintf.h
  threader.cpp
  threader.h
  timerfd.cpp
  timerfd.h
  #when did this functionality disappear? what was it? vargs.cpp
  worstcase.cpp
  worstcase.h
  platform.cpp
)


target_include_directories(${safely_target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

get_target_property(arf ${safely_target} "INCLUDE_DIRECTORIES")
message("posix includes: ${arf}")
