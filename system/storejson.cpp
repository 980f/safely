#include "storejson.h"
#include "utf8.h"

StoreJSON::StoreJSON()
{

}

void StoreJSON::parse(char *buffer, int length, Storable &parent)
{
 CharScanner lookahead(buffer,length);
 //skip whitespace

 while (lookahead.hasNext()) {
 UTF8 ch=lookahead.next()  ;

 }
 //expect name or value
}
