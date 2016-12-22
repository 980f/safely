#include "storejson.h"
#include "utf8.h"


static
UTF8 skipwhite(CharScanner &scanner){
  while (scanner.hasNext()) {//space leading name or value
    UTF8 ch=scanner.next()  ;
    if(!ch.isWhite()){
      return ch;
    }
  }
  return 0;
}


JSONparser::JSONparser(char *buffer, unsigned length, Storable &parent):
  token(buffer,length),
  lookahead(buffer,length),
  parent(parent)
{
  parse();
}

void JSONparser::parse(){
 //expect name or value

}
