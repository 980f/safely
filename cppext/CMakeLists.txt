
message("including cppext")

target_sources(${safely_target} PUBLIC
  bigendianer.cpp
  bitbanger.cpp
  bitlock.cpp
  bitwise.cpp
  bufferformatter.cpp
  centeredslice.cpp
  char.cpp
  charformatter.cpp
  char.cpp
  charscanner.cpp
  cheaptricks.cpp
  continuedfractionratiogenerator.cpp
  converter.cpp
  core-atomic.cpp
  countdown.cpp
  crc16m.cpp
  cstr.cpp
  cycler.cpp
  determinator.cpp
  enumerated.cpp
  enumerizer.cpp
  fitstat.cpp
  halfopen.cpp
  linearfit.cpp
  logger.cpp
  maxtracked.cpp
  minimath.cpp
  multilock.cpp
  numberformat.cpp
  numberparser.cpp
  numberpieces.cpp
  numericalvalue.cpp
  pathiterator.cpp
  pathname.cpp
  poissonfeeder.cpp
  polledtimer.cpp
  polyfilter.cpp
  polynomial.cpp
  pushedjsonparser.cpp
  pushedparser.cpp
  quadraticfilter.cpp
  quantizer.cpp
  ranged.cpp
  realstatistic.cpp
  safestr.cpp
  softpwm.cpp
  textchunk.cpp
  textkey.cpp
  textwalker.cpp
  transactionalbuffer.cpp
  twiddler.cpp
  unionizer.cpp
  ustring.cpp
  utf8.cpp
  toker.cpp
)

target_include_directories(${safely_target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

get_target_property(arf ${safely_target} "INCLUDE_DIRECTORIES")
message("cppext includes: ${arf}")
