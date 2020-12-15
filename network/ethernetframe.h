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

constexpr uint16_t htons(uint16_t swapme) {
  return swapme >> 8 | swapme << 8;
}

extern "C" uint32_t htonl(uint32_t littlendian);

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
    copy(rhs.octets);
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
    return copy(rhs.octets);
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
  EthernetAddress hardwareAddress;
  IPV4Address protocolAddress;
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
  unsigned headerLength: 4; //*4 for bytes
  unsigned version: 4;

  uint8_t DSCP_ECN;//: 6;   unsigned ECN: 2;

  uint16_t totalLength;//576 = 512+64
  uint16_t uselessID;
// ignore fragmentation, helps get correct size for struct.
//  struct FragmentationStuff {
//    unsigned fragmentOffset: 13;
//    unsigned moreFragments: 1;
//    unsigned dontFragme: 1;
//    unsigned ff0: 1;//ignored
//  } PACKED;
//  FragmentationStuff
  uint16_t fragger;//a FragmentationStuff, union was causing pain when constexpr init'ing

  uint8_t timeToLive;
  uint8_t protocolCode;
  uint16_t headerChecksum;
  IPV4Address sourceIP;
  IPV4Address destIP;

  void addToLength(unsigned morebytes){
    totalLength = htons(htons(totalLength) + morebytes);
  }

} PACKED;

using AckValue = uint32_t;

struct TCPHeader {
  uint16_t sourceport;
  uint16_t destport;

  AckValue seq;
  AckValue ack;

  unsigned : 4;//reserved
  unsigned offset: 4;//aka headerLength NB *4 to get byte offset of data from end of header

    unsigned FIN: 1;
    unsigned SYN: 1;
    unsigned RST: 1;
    unsigned PSH: 1;
    unsigned ACK: 1;
    unsigned URG: 1;
    unsigned ECE: 1;
    unsigned CWR: 1;


  uint16_t window;
  uint16_t checksum;
  uint16_t urgent;
} PACKED;



constexpr unsigned tcpDataOffset = sizeof(IPV4Header) + sizeof(TCPHeader);  //40


struct TcpEthernet {
  EthernetHeader ethernetHeader;
  IPV4Header ipv4Header;
  TCPHeader tcpHeader;
  uint8_t etc[1500]; //bigger than we will need.
  /** options follow header, might be zero length
  if offset(IHL)>5 then there will be more bytes following this object.
  options are byte streamy, 8 bit commands, null terminated.
  struct Option {
    unsigned code:5; // 11 for MTU probe, 12 for MTU reply
    unsigned debugger:1; //else control option
    unsigned skip1:1;//reserved
    unsigned copy:1; //fragmenter must copy this option to all fragments
  };
*/
  DataBlock options();

public:

  /** payload follows options, which might be zero length and ditto for the payload. */
  DataBlock payload();
//  void loadData(const DataBlock &block);
  uint8_t *paystart() ;
  unsigned actualLength();
}PACKED;
