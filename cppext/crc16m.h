#ifndef CRC16M_H
#define CRC16M_H
#include "buffer.h"
#include "eztypes.h"
/**
compute a 16 bit CRC from an octet stream using the polynomial defined by Modbus.
*/
class Crc16m {
public:
  static u16 compute(Indexer <u8>&summer);
private: //someday might make a state based one ...
  Crc16m();
};

#endif // CRC16M_H
