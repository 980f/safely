
target_sources(${safely_target} PUBLIC
  serveroptions.cpp
  serveroptions.h
  #socketeer.cpp
  socketeer.h
  #telnetserver.cpp
  telnetserver.h
  #tcpsocket.cpp
  tcpserver.cpp
)

target_include_directories(${safely_target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

get_target_property(arf ${safely_target} "INCLUDE_DIRECTORIES")
message("network includes: ${arf}")
