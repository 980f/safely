#include "storejson.h"

StoreJSON::StoreJSON()
{

}

void StoreJSON::parse(const char *buffer, int length, Storable &parent)
{
 CharScanner lookahead();
 //skip whitespace

 while (scanner.hasNext()) {
 UTF8 ch=scanner.next()  ;

 }
 //expect name or value
}
