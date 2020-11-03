// (C) 2020 Andrew Heilveil (github/980f) Created on 10/30/20.
#pragma once

#include <stdint.h>
#include <cstr.h>
#include <char.h>
#include <charscanner.h>
#include "bigendianer.h"

//on the cortexM series this will inline a single instruction.
extern constexpr void htonl32(uint8_t *target, uint32_t source);

template<unsigned num> class NetworkAddress {
protected:
  /** maintained in network order, MSbyte at lowest index */
  uint8_t octets[num];

public:
  BigEndianer scanner() {
    return {octets, sizeof(octets)};
  }

  void parse(Cstr humanreadable, bool hexly) {
    ByteScanner reader(humanreadable.casted(), humanreadable.length());
    auto writer(scanner());
    while (writer.hasNext()) {
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
};

struct EthernetAddress : public NetworkAddress<6> {
  /** parse 6 2 char hex fields separate by colons (or any non hexadecimal digit) */
  void parse(Cstr humanreadable) {
    NetworkAddress<6>::parse(humanreadable, true);
  }

  static const EthernetAddress Broadcast;//todo:00 how to make a const one?
};

struct IPV4Address : public NetworkAddress<4> {
  IPV4Address(uint32_t raw) {
    htonl32(octets, raw);
  }

  /** parse dotted decimal */
  void parse(Cstr humanreadable) {
    NetworkAddress<4>::parse(humanreadable, false);
  }
};

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

struct IPV4Header {
  unsigned version: 4;
  unsigned headerLength: 4; //*4 for bytes
  unsigned DSCP: 6;
  unsigned ECN: 2;

  uint16_t totalLength;//576 = 512+64
  uint16_t uselessID;
  unsigned ff0: 1;//ignored
  unsigned dontFragme: 1;
  unsigned moreFragments: 1;
  unsigned fragmentOffset: 13;
  uint8_t timeToLive;
  uint8_t protocolCode;
  uint16_t headerChecksum;
  IPV4Address sourceIP;
  IPV4Address destIP;
  //if IHL>5 then there will be more bytes following this object.
  //options are byte streamy, 8 bit commands, null terminated.
//  struct Option {
//    unsigned code:5; // 11 for MTU probe, 12 for MTU reply
//    unsigned debugger:1; //else control option
//    unsigned skip1:1;//reserved
//    unsigned copy:1; //fragmenter must copy this option to all fragments
//  };
};