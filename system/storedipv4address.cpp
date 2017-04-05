#include "storedipv4address.h"
#include "stdlib.h" //strtol

#include "textformatter.h"

StoredIPV4Address::StoredIPV4Address(Storable &node) : Stored(node),
  ConnectChild(resolved),
  ConnectChild(dotted){
  resolved.onAnyChange(MyHandler(StoredIPV4Address::makeText));
  dotted.onAnyChange(MyHandler(StoredIPV4Address::makeNumber),true);//on load conflict let dotted win.
}

Text StoredIPV4Address::dotstring(u32 ipv4){
  u32 accumulator(ipv4);
  u8 bytes[4];
  for(int i = 4; i-->0; ) {
    bytes[i] = accumulator >> 24;
    accumulator <<= 8;
  }
  return TextFormatter::compose("%1.%2.%3.%4",bytes[3],bytes[2],bytes[1],bytes[0]);
//  Text workspace("999.999.999.999");//todo: fix textformatter or appy bufferformatter
//  return workspace;//TextFormatter::compose("%1.%2.%3.%4",bytes[3],bytes[2],bytes[1],bytes[0]);
}

void StoredIPV4Address::makeText(){
  dotted = dotstring(resolved.native());
}

void StoredIPV4Address::makeNumber(){
  char *separator = Cstr::violate(dotted.c_str());//strtol is missing a const
  u32 accumulator = 0;
  while(true) {
    long int octet = strtol(separator, &separator,10);
    accumulator <<= 8;
    accumulator += octet;
    if(!separator|| *separator++!='.') {
      break;
    }
  }
  resolved = accumulator;
} // StoredIPV4Address::makeNumber
