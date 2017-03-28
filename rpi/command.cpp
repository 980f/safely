#include "command.h"

#define STX 2
#define ETX 3

Command::Command():
  parsed(',',""),
  receiver(raw,sizeof(raw))
{
  receiver.zguard();
}

bool Command::validCommand(){
  CharScanner p(receiver,~0);
  parsed.clear();
  parsed.parse(receiver.internalBuffer());
  if(parsed.quantity()>=2){
    Text sumImage=*parsed.last();
    p.rewind();
    char csum=checksum(p);
    return csum=='@';//0 coded as ascii
  } else {
    //no checksum
    return false;
  }
  return false;
}

bool Command::commandPresent(){
  CharScanner p(receiver,~0);

  while(p.hasNext()){
    char c=p.next();
    switch(c){
    case STX:
      receiver.remove(receiver.used());
      break;
    case ETX:
      p.previous()=0;//replace etx with eos.
      return validCommand();
      //      break;
    }
  }
  return false;//ran off end of data without seeing terminator
}

char Command::checksum(CharScanner p){
  char sum=0;
  while(p.hasNext()){
    sum+=p.next();
  }
  sum = (-sum) & 0x3F;//complement and clip to 6 bits
  sum |= '@';//bump into alpha range
  return sum;
}

