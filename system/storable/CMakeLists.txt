
message("including 'storable' ")
target_sources(${safely_target} PUBLIC
  #cachedindextostored.cpp
  #cachedindextostored.h
  jsonstore.cpp
  jsonstore.h
  #needs ArtFile kanjidic.cpp
  #kanjidic.h
  storable.cpp
  storable.h
  storageexporter.cpp
  storageexporter.h
  stored.cpp
  storedenum.cpp
  storedenum.h
  storedgroup.cpp
  storedgroup.h
  stored.h
  storedlabel.cpp
  storedlabel.h
  storednumeric.cpp
  storednumeric.h
  storedsettable.h
  storejson.cpp
  storejson.h
  #needs working file path nametreefile.cpp
  #treefile.h
)

target_include_directories(${safely_target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

get_target_property(arf ${safely_target} "INCLUDE_DIRECTORIES")
message("storable includes: ${arf}")
