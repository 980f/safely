
add_library(rasbpi STATIC
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

target_include_directories(rasbpi PUBLIC
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/cppext
  ${PROJECT_SOURCE_DIR}/system
  ${PROJECT_SOURCE_DIR}/posix
  ${CMAKE_CURRENT_SOURCE_DIR}
)

get_target_property(arf rasbpi "INCLUDE_DIRECTORIES")
message("rasbpi includes ${arf}")
