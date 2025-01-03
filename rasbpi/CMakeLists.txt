
target_sources(${safely_target} PUBLIC
  clockgenerator.cpp
  clockgenerator.h
  din.cpp
  din.h
  dout.cpp
  dout.h
  gpio.cpp
  gpio.h
  i2c.cpp
  i2c.h
  inputmonitor.cpp
  inputmonitor.h
  peripheral.cpp
  peripheral.h
  piinfo.cpp
  piinfo.h
  pwm.cpp
  pwm.h
  SSD1306.cpp
  SSD1306.h
)

target_include_directories(${safely_target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

get_target_property(arf ${safely_target} "INCLUDE_DIRECTORIES")
message("rasbpi includes: ${arf}")
