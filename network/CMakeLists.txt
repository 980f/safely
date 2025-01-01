
target_sources(${safely_target} PUBLIC
  tcpsocket.cpp
  tcpserver.cpp
)

target_include_directories(${safely_target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

get_target_property(arf ${safely_target} "INCLUDE_DIRECTORIES")
message("cppext includes: ${arf}")
