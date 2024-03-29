#include "storedipv4address.h"
#include "stdlib.h" //strtol

#include "textformatter.h" //todo:1 replace with bufferformatter

StoredIPV4Address::StoredIPV4Address(Storable &node) : Stored(node),
  ConnectChild(resolved),
  ConnectChild(dotted){
  resolved.onAnyChange(MyHandler(StoredIPV4Address::makeText));
  dotted.onAnyChange(MyHandler(StoredIPV4Address::makeNumber),true);//on load conflict let dotted win.
}

Text StoredIPV4Address::dotstring(u32 ipv4){
  u8 *bytes=reinterpret_cast<u8*>(&ipv4);
  return TextFormatter::compose("$0.$1.$2.$3",u64(bytes[3]),u64(bytes[2]),u64(bytes[1]),u64(bytes[0]));
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
