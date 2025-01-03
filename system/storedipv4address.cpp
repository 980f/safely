#include "storedipv4address.h"
#include <cstdlib> //strtol

#include "textformatter.h" //todo:1 replace with bufferformatter

StoredIPV4Address::StoredIPV4Address(Storable &node) : Stored(node),
  ConnectChild(resolved),
  ConnectChild(dotted){
  resolved.onAnyChange(MyHandler(StoredIPV4Address::makeText));
  dotted.onAnyChange(MyHandler(StoredIPV4Address::makeNumber),true);//on load conflict let dotted win.
}

Text StoredIPV4Address::dotstring(uint32_t ipv4){
  auto *bytes=reinterpret_cast<uint8_t*>(&ipv4);
  return TextFormatter::compose("$0.$1.$2.$3",uint64_t(bytes[3]),uint64_t(bytes[2]),uint64_t(bytes[1]),uint64_t(bytes[0]));//cast needed to get correct format, else would be treated as a character, not a number 0..255.
}

void StoredIPV4Address::makeText(){
  dotted = dotstring(resolved.native());
}

void StoredIPV4Address::makeNumber(){
  char *separator = Cstr::violate(dotted.c_str());//strtol is missing a const
  uint32_t accumulator = 0;
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
