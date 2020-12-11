// (C) 2020 Andrew Heilveil (github/980f) Created on 10/30/20.

#include "ethernetframe.h"

DataBlock TcpEthernet::options() {
  return {(tcpHeader.offset-5) * 4u, reinterpret_cast<uint8_t *>(&tcpHeader) + sizeof(TCPHeader)};
}

DataBlock TcpEthernet::payload(unsigned int totalEthernetLength) {
  auto opts=options();
  return {totalEthernetLength - sizeof(TcpEthernet) -opts.quantity(), opts.violated()+opts.quantity()};//todo:00 this presumes checksum has been stripped from given length
}

void TcpEthernet::loadData(const DataBlock &block) {
  auto opts=options();
  memcpy(opts.violated()+opts.quantity(),block.violated(),block.quantity());
  ipv4Header.totalLength+=block.quantity();
}

