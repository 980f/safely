#include "storedipv4address.h"

StoredIPV4Address::StoredIPV4Address(Storable &node):Stored(node),
  ConnectChild(resolved),
  ConnectChild(dotted)
{
  resolved.onAnyChange(MyHandler(StoredIPV4Address::makeText));
  dotted.onAnyChange(MyHandler(StoredIPV4Address::makeNumber),true);//on load conflict let dotted win.
}

Glib::ustring StoredIPV4Address::dotstring(u32 ipv4){
  u32 accumulator=ipv4;
  u8 bytes[4];
  for(int i=4;i-->0;){
    bytes[i]=accumulator>>24;
    accumulator<<=8;
  }
  return Glib::ustring::compose("%1.%2.%3.%4",bytes[3],bytes[2],bytes[1],bytes[0]);
}

void StoredIPV4Address::makeText() {  
  dotted=dotstring(resolved.native());
}

void StoredIPV4Address::makeNumber(){
  const char *separator=dotted.c_str();
  u32 accumulator=0;
  while(true){
    long int octet=strtol(separator,const_cast<char **>( &separator),10);
    accumulator<<=8;
    accumulator+=octet;
    if(!(separator&&*separator++=='.')){
      break;
    }
  }
  resolved=accumulator;
}
