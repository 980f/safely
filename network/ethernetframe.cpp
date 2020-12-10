// (C) 2020 Andrew Heilveil (github/980f) Created on 10/30/20.

#include "ethernetframe.h"

DataBlock TcpEthernet::options() {
  return {tcpHeader.offset * 4u, reinterpret_cast<uint8_t *>(&tcpHeader) + sizeof(TCPHeader)};
}

DataBlock TcpEthernet::payload(unsigned int totalEthernetLength) {
  auto opts=options();
  return {totalEthernetLength - sizeof(TcpEthernet) -opts.quantity(), opts.violated()+opts.quantity()};//todo:00 this presumes checksum has been stripped from given length
}

Block<uint8_t> IPV4Header::options() {
  if (headerLength > 5) {
    return {chunkLength(headerLength, 5), myWord(5)};
  } else {
    return {};
  }
}
