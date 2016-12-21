#include "utf8text.h"
#include "utf8.h"

Utf8Text::Utf8Text()
{

}

int Utf8Text::encodedLength(Cstr utf8s)
{
  if(utf8s.empty()){
    return 0;
  }
  int totes(0);
  const char *scan=utf8s;
  while(UTF8 ch=*scan++){
    unsigned followers=ch.numFollowers();
    //skip the followers
    if(followers>0){

    }
  }

 //for()
  return totes;
}
