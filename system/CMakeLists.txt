
message("including 'system' ")
target_sources(${safely_target} PUBLIC
  allocatedindexer.cpp
  allocatedindexer.h
  block.h
  #cachedindextostored.cpp
  #cachedindextostored.h
  chain.h
  chainsorter.h
  delimitedinputstream.cpp
  delimitedinputstream.h
  delimitingoutputstream.cpp
  delimitingoutputstream.h
  dottedname.cpp
  dottedname.h
  dyndeterminator.cpp
  dyndeterminator.h
  filenameconverter.cpp
  filenameconverter.h
  filename.cpp
  filename.h
  gatedsignal.cpp
  gatedsignal.h
  jsonstore.cpp
  jsonstore.h
  #needs ArtFile kanjidic.cpp
  #kanjidic.h
  linearfilter.cpp
  linearfilter.h
  managedblock.cpp
  managedblock.h
  mapiterator.h
  matrixinverter.cpp
  matrixinverter.h
  #formatter hierarchy seems to have evaporated measure.cpp
  #measure.h
  numberformatter.cpp
  numberformatter.h
  pathparser.cpp
  pathparser.h
  peakfindrecords.cpp
  peakfindrecords.h
  runoncenthtime.cpp
  runoncenthtime.h
  segmentedname.cpp
  segmentedname.h
  sigcuser.cpp
  sigcuser.h
  storable.cpp
  storable.h
  storageexporter.cpp
  storageexporter.h
  storedcorrelation.cpp
  storedcorrelation.h
  stored.cpp
  storedenum.cpp
  storedenum.h
  storedgroup.cpp
  storedgroup.h
  stored.h
  storedipv4address.cpp
  storedipv4address.h
  storedlabel.cpp
  storedlabel.h
  storednumeric.cpp
  storednumeric.h
  storedrange.cpp
  storedrange.h
  storedsettable.h
  storejson.cpp
  storejson.h
  textformatter.cpp
  textformatter.h
  textpointer.cpp
  textpointer.h
  #needs working file path nametreefile.cpp
  #treefile.h
  urltext.cpp
  urltext.h
  utf8converter.cpp
  utf8converter.h
  utf8text.cpp
  utf8text.h
  utf8transcoder.cpp
  utf8transcoder.h
  watchable.h
)

target_include_directories(${safely_target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

get_target_property(arf ${safely_target} "INCLUDE_DIRECTORIES")
message("cppext includes: ${arf}")
