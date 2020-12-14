// (C) 2020 Andrew Heilveil (github/980f) Created on 10/30/20.

#include "ethernetframe.h"

DataBlock TcpEthernet::options() {
  return {(tcpHeader.offset - 5) * 4u, reinterpret_cast<uint8_t *>(&ethernetChecksum)};
}

DataBlock TcpEthernet::payload() {
  auto opts = options();
  auto paystart = opts.quantity() > 0 ? opts.violated() + opts.quantity() : reinterpret_cast<uint8_t *>(&ethernetChecksum);
  return {htons(ipv4Header.totalLength)-40u , paystart};//40 ~= sizeof(IpHeader)+sizeof(TcpHeader)
}

void TcpEthernet::loadData(const DataBlock &block) {
  auto opts = options();
  memcpy(opts.violated() + opts.quantity(), block.violated(), block.quantity());
  ipv4Header.totalLength += block.quantity();
}

