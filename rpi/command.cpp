#include "command.h"

#define STX 2
#define ETX 3

Command::Command():
  receiver(raw,sizeof(raw)), //no null terminator, not ascii
  args(params,sizeof(params))
{

}

bool Command::validCommand(){
  ByteScanner p(receiver,~0);

  u8 sum=0;

  return  false;
}

bool Command::commandPresent(){
  ByteScanner p(receiver,~0);

  while(p.hasNext()){
    u8 c=p.next();
    switch(c){
    case STX:
      receiver.remove(receiver.used());
      break;
    case ETX:
      return validCommand();
      break;
    default:
      break;
    }
  }
}

//bool Command::parse(u8 byte)
//{
// if(byte==2){
//   receiver.rewind();
//   return false;
// }
// if(byte==3){
//   //check body
//   while(p.hasNext()){
//     sum+=p.next();
//   }
//   sum &=0x3f;
//   sum |=0x40;
//   if(sum==0){
//     p.rewind();
//     args.rewind();
//     //parse args
//   }
// }
//}

ArgSet Command::takeArgs(bool rewind){
  ArgSet used(args,~0);
  if(rewind){
    args.rewind();
  }
  return used;
}
