// (C) 2020 Andrew Heilveil (github/980f) Created on 10/30/20.
#pragma once

#include <stdint.h>
#include "block.h"
#include <cstr.h>
#include <char.h>
#include <charscanner.h>
#include "bigendianer.h"
#include <cstring> //temp for memcpy
#include <cgmessage.h>


constexpr uint16_t htons(uint16_t swapme){
  return swapme>>8 | swapme<<8;
}

template<unsigned num> class NetworkAddress {
protected:
  /** maintained in network order, MSbyte at lowest index */
  uint8_t octets[num];

  INLINETHIS
  NetworkAddress &copy(const uint8_t *rhs) {
    memcpy(octets, rhs, num);
    return *this;
  }

public:
  NetworkAddress(const NetworkAddress<num> &rhs) {
    copy(rhs);
  }

  NetworkAddress(const uint8_t rhs[num]) {
    copy(rhs);
  }

  /** garbage, unless in the block lzeroed by c startup*/
  NetworkAddress() {
  }

  NetworkAddress &operator=(const uint8_t *rhs) {
    return copy(rhs);
  }

  NetworkAddress &operator=(const NetworkAddress<num> &rhs) {
    return copy(rhs.octets);
  }

  NetworkAddress &operator=(NetworkAddress<num> &&rhs) {
    return copy(rhs);
  }

  BigEndianer scanner() {
    return {octets, num};
  }

  void parse(Cstr humanreadable, bool hexly) {
    ByteScanner reader(humanreadable.casted(), humanreadable.length());
    for (auto writer(scanner()); writer.hasNext();) {//#IDE mismarks this as not looping
      auto &octet = writer.next();
      octet = 0;
      while (reader.hasNext()) {
        Char c(reader.next());
        if (!(hexly ? c.appliedNibble(octet) : c.appliedDigit(octet))) {
          break;
        }
      }
    }
  };

  bool operator==(const NetworkAddress<num> &sameclass) const {
    for (unsigned length = num; length-- > 0;) {
      if (octets[length] != sameclass.octets[length]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const NetworkAddress<num> &sameclass) const {
    return !operator==(sameclass);
  }
} PACKED;

struct EthernetAddress : public NetworkAddress<6> {
  /** parse 6 2 char hex fields separate by colons (or any non hexadecimal digit) */
  void parse(Cstr humanreadable) {
    NetworkAddress<6>::parse(humanreadable, true);
  }
} PACKED;

struct IPV4Address : public NetworkAddress<4> {
//  IPV4Address(uint32_t raw) :NetworkAddress<4>(&pun(uint8_t,raw)){//todo:00 htonlize the argument!
//    //
//  }
//
  /** parse dotted decimal */
  void parse(Cstr humanreadable) {
    NetworkAddress<4>::parse(humanreadable, false);
  }
}PACKED;

struct ArpV4Pair {
  /** carefully layed out for direct use in packets */
  EthernetAddress HA;
  IPV4Address PA;
} PACKED;

enum EtherType {//if these insert wrong into packets then fix that here, no need to be byteswapping constants.
  NoType = 0 //indicates uninit field.
  , ARP_type = htons(0x0806)
  , IPV4_type = htons(0x0800) //IPV4
  , RARP_type = htons(0x8035) // Reverse ARP
  , PTP = htons(0x88F7)  //Precision Time Protocol
};

/** ARP opcodes */
enum Arpcodes {
  NoCode = 0 //indicates uninit field.
  , ArpRequest = htons(1)
  , ArpReply = htons(2)
  // 3 .. 9 for RARP and InARP and friends, add as needed.
};

enum IPProtos {
  NoProtocol = 0
  , ICMP = 1   //ping
  , TCP = 6  //streamy
  , UDP = 17 //chunky
  , Experimental = 253
  , Testing = 254
};

/** the 14 byte header */
struct EthernetHeader {
  EthernetAddress destMac;
  EthernetAddress sourceMac;
  EtherType ethType;
} PACKED;

/** arp content
 *
 * FYI: gratuitous ARP: targetPA=senderPA targetHA=zip;
 * probe: senderPA&HA=self; target HA=zip and target PA is what we are looking for the address of.
 *
 * */
struct ArpIPV4 {
  uint16_t hardwareType;// = htons(1); //wifi htons(6)
  uint16_t protocolType;// = htons(2048);
  uint8_t hwAddressLength;// = 6;
  uint8_t protocolAddressLength;// = 4;
  uint16_t operation;
  ArpV4Pair sender;
  ArpV4Pair target;

} PACKED;


/** complete arp packet */
struct ArpIPV4Ethernet {
  EthernetHeader ethernetHeader;
  ArpIPV4 arpIpv4;
} PACKED;

/** */
struct IPV4Header {
  uint16_t totalLength;//576 = 512+64

  unsigned DSCP: 6;
  unsigned ECN: 2;
  unsigned headerLength: 4; //*4 for bytes
  unsigned version: 4;

  union {
    struct {
      unsigned fragmentOffset: 13;
      unsigned moreFragments: 1;
      unsigned dontFragme: 1;
      unsigned ff0: 1;//ignored
    };
    uint16_t packed;
  } fragger;
  uint16_t uselessID;

  uint16_t headerChecksum;
  uint8_t protocolCode;
  uint8_t timeToLive;

  IPV4Address sourceIP;
  IPV4Address destIP;

protected:
  uint8_t *myWord(unsigned int wordOffset) {
    return (reinterpret_cast<uint8_t *>(this) + 4 * wordOffset);
  }

  unsigned chunkLength(uint16_t total, unsigned int header) const {
    return static_cast<unsigned int>(4 * (total - header));
  }

public:
  /** @returns pointer to bytes following base part of header */
  Block<uint8_t> options();

/** @returns length of and pointer to bytes following base part of header */
  Block<uint8_t> payload() {
    return {chunkLength(totalLength, headerLength), myWord(headerLength)};
  }
  //if IHL>5 then there will be more bytes following this object.
  //options are byte streamy, 8 bit commands, null terminated.
//  struct Option {
//    unsigned code:5; // 11 for MTU probe, 12 for MTU reply
//    unsigned debugger:1; //else control option
//    unsigned skip1:1;//reserved
//    unsigned copy:1; //fragmenter must copy this option to all fragments
//  };
} PACKED;

using AckValue = uint32_t;

struct TCPHeader {
  uint16_t destport;
  uint16_t sourceport;
  AckValue seq;
  AckValue ack;
  uint16_t window;
  struct {
    unsigned FIN: 1;
    unsigned SYN: 1;
    unsigned RST: 1;
    unsigned PSH: 1;
    unsigned ACK: 1;
    unsigned URG: 1;
    unsigned ECE: 1;
    unsigned CWR: 1;
  } f;
  unsigned : 4;//reserved
  unsigned offset: 4;//NB *4 to get byte offset of data from end of header

  uint16_t urgent;
  uint16_t checksum;
} PACKED;

struct TcpEthernet {
  EthernetHeader ethernetHeader;
  IPV4Header ipv4Header;
  TCPHeader tcpHeader;

  /** options follow header, might be zero length */
  DataBlock options();

  /** payload follows options, which might be zero length and ditto for the payload. */
  DataBlock payload(unsigned totalEthernetLength);
}PACKED;
