#cmake_minimum_required(VERSION 3.16)
#project(cppext VERSION 1.0 LANGUAGES CXX)

message("including cppext")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(cppext STATIC
  abstractjsonparser.h
  abstractpin.h
  bigendianer.cpp
  bigendianer.h
  bitbanger.cpp
  bitbanger.h
  bitlock.cpp
  bitlock.h
  bitwise.cpp
  bitwise.h
  bufferformatter.cpp
  bufferformatter.h
  buffer.h
  centeredslice.cpp
  centeredslice.h
  chained.h
  changemonitored.h
  char.cpp
  charformatter.cpp
  charformatter.h
  char.h
  charscanner.cpp
  charscanner.h
  cheaptricks.cpp
  cheaptricks.h
  circularbuffer.h
  circularindexer.h
  continuedfractionratiogenerator.cpp
  continuedfractionratiogenerator.h
  converter.cpp
  converter.h
  core-atomic.cpp
  core-atomic.h
  countdown.cpp
  countdown.h
  cppexttester.h
  crc16m.cpp
  crc16m.h
  cstr.cpp
  cstr.h
  cycler.cpp
  cycler.h
  debuggable.h
  demonic.h
  determinator.cpp
  determinator.h
  enumerated.cpp
  enumerated.h
  enumerizer.cpp
  enumerizer.h
  eventflag.h
  extremer.h
  eztypes.h
  fitstat.cpp
  fitstat.h
  halfopen.cpp
  halfopen.h
  hook.h
  index.h
  iterate.h
  linearfit.cpp
  linearfit.h
  linearsmoother.h
  linearstatistic.h
  lock.h
  maxtracked.cpp
  maxtracked.h
  minimath.cpp
  minimath.h
  multilock.cpp
  multilock.h
  numberformat.cpp
  numberformat.h
  numberparser.cpp
  numberparser.h
  numberpieces.cpp
  numberpieces.h
  numericalvalue.cpp
  numericalvalue.h
  onexit.h
  ordinator.h
  pathiterator.cpp
  pathiterator.h
  pathname.cpp
  pathname.h
  poissonfeeder.cpp
  poissonfeeder.h
  polledtimer.cpp
  polledtimer.h
  polyfilter.cpp
  polyfilter.h
  polynomial.cpp
  polynomial.h
  pushedjsonparser.cpp
  pushedjsonparser.h
  pushedparser.cpp
  pushedparser.h
  quadraticfilter.cpp
  quadraticfilter.h
  quantizer.cpp
  quantizer.h
  ranged.cpp
  ranged.h
  range.h
  realstatistic.cpp
  realstatistic.h
  safestr.cpp
  safestr.h
  scaler.h
  sequence.h
  softpwm.cpp
  softpwm.h
  syncretic.h
  tableofpointers.h
  textchunk.cpp
  textchunk.h
  textkey.cpp
  textkey.h
  textwalker.cpp
  textwalker.h
  transactionalbuffer.cpp
  transactionalbuffer.h
  twiddler.cpp
  twiddler.h
  unionizer.cpp
  unionizer.h
  ustring.cpp
  ustring.h

  utf8.cpp
  utf8.h
)

#
#target_include_directories (cppext PUBLIC
#  ${PROJECT_SOURCE_DIR}/cppext
#)

message( "in cppext project is ${PROJECT_SOURCE_DIR}")

target_include_directories(cppext PUBLIC
  ${PROJECT_SOURCE_DIR}

)
