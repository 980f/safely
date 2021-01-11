// (C) 2020 Andrew Heilveil (github/980f) Created on 10/30/20.

#include "ethernetframe.h"

DataBlock TcpEthernet::options() {
  return {(tcpHeader.offset - 5) * 4u, reinterpret_cast<uint8_t *>(&etc)};
}

DataBlock TcpEthernet::payload() {
  return {htons(ipv4Header.totalLength) - tcpDataOffset, paystart()};//40 ~= sizeof(IpHeader)+sizeof(TcpHeader)
}

uint8_t *TcpEthernet::paystart() {
  auto paystart = reinterpret_cast<uint8_t *>(&etc);
  auto optquantity = (tcpHeader.offset - 5) * 4u;
  return paystart + optquantity;
}

unsigned TcpEthernet::actualLength() {
  return sizeof(ethernetHeader) + htons(ipv4Header.totalLength);
}


