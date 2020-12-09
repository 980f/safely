// (C) 2020 Andrew Heilveil (github/980f) Created on 10/30/20.
#pragma once

#include <stdint.h>
#include "block.h"
#include <cstr.h>
#include <char.h>
#include <charscanner.h>
#include "bigendianer.h"
#include <cstring> //temp for memcpy

//on the cortexM series this will inline a single instruction.
extern constexpr void htonl32(uint8_t *target, uint32_t source);


template<unsigned num> class NetworkAddress {
protected:
  /** maintained in network order, MSbyte at lowest index */
  uint8_t octets[num];

public:
  NetworkAddress(const uint8_t *octets) {
    memcpy(this->octets, octets, num);
  }

  void makeall(bool ones) {
    memset(octets, ones ? ~0 : 0, num);
  }

  BigEndianer scanner() {
    return {octets, sizeof(octets)};
  }

  void parse(Cstr humanreadable, bool hexly) {
    ByteScanner reader(humanreadable.casted(), humanreadable.length());
    auto writer(scanner());
    while (writer.hasNext()) {//#IDE mismarks this
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
  /** carefully layed out for EthernetFrame */
  EthernetAddress HA;
  IPV4Address PA;
} PACKED;

enum HTYPE {
  Ethernet = 1
};

enum EtherType {
  NoType = 0 //indicates uninit field.
  , ARP_type = 0x0806
  , IPV4_type = 0x0800 //APV4
  , RARP_type = 0x8035 // Reverse ARP
  , PTP = 0x88F7  //Precision Time Protocol
};

/** ARP opcodes */
enum Arpcodes {
  NoCode = 0 //indicates uninit field.
  , ArpRequest = 1
  , ArpReply = 2
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
/**
 * gratuitous ARP: targetPA=senderPA targetHA=zip;
 * probe: senderPA&HA=self; target HA=zip and target PA is what we are looking for the address of.
 *
 * */
struct ArpIPV4_Ether {
  uint16_t hardwareType = 1; //wifi 6
  uint16_t protocolType = 2048;
  uint8_t hwAddressLength = 6;
  uint8_t protocolAddressLength = 4;
  uint16_t operation;
  ArpV4Pair sender;
  ArpV4Pair target;
} PACKED;

struct EthernetHeader {
  EthernetAddress destMac;
  EthernetAddress sourceMac;
  EtherType ethType;
} PACKED;


/***/
struct IPV4Header {
  uint16_t totalLength;//576 = 512+64
  unsigned DSCP: 6;
  unsigned ECN: 2;
  unsigned headerLength: 4; //*4 for bytes
  unsigned version: 4;


  unsigned fragmentOffset: 13;
  unsigned moreFragments: 1;
  unsigned dontFragme: 1;
  unsigned ff0: 1;//ignored
  uint16_t uselessID;

  uint16_t headerChecksum;
  uint8_t protocolCode;
  uint8_t timeToLive;

  IPV4Address sourceIP;
  IPV4Address destIP;

protected:
  uint8_t *myWord(unsigned int wordOffset) { return (reinterpret_cast<uint8_t *>(this) + 4 * wordOffset); }

  unsigned chunkLength(uint16_t total, unsigned int header) const { return static_cast<unsigned int>(4 * (total - header)); }

public:
  /** @returns pointer to bytes following base part of header */
  Block<uint8_t> options() {
    if (headerLength > 5) {
      return {chunkLength(headerLength, 5), myWord(5)};
    } else {
      return {};
    }
  }


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
  unsigned FIN: 1;
  unsigned SYN: 1;
  unsigned RST: 1;
  unsigned PSH: 1;
  unsigned ACK: 1;
  unsigned URG: 1;
  unsigned ECE: 1;
  unsigned CWR: 1;
  unsigned : 4;//reserved
  unsigned offset: 4;//*4 to get byte offset of data from end of header
  uint16_t urgent;
  uint16_t checksum;

  uint8_t *options() {
    return reinterpret_cast<uint8_t *>(this) + sizeof(TCPHeader);
  };

  uint8_t *data() {
    return options() + offset * 4;
  }

};