#pragma once

#include "buffer.h"
#include <cstdint>
/**
compute a 16 bit CRC from an octet stream using the polynomial defined by Modbus.
*/
class Crc16m {
public:
  static uint16_t compute(Indexer <uint8_t>&summer);
private: //someday might make a state based one ...
  Crc16m();
};
