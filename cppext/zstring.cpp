#include "zstring.h"

#include "string.h"
#include "malloc.h"
#include "textkey.h"

Zstring::Zstring(char *str):str(str)
{

}

int Zstring::len() const
{
  return str?strlen(str):0;
}

char *Zstring::chr(int chr) const
{
  if(str){
    return strchr(str,chr);
  } else {
    return nullptr;
  }
}

int Zstring::cmp(const char *rhs) const
{
  if(str){
    if(rhs){
      return strcmp(str,rhs);
    } else {
      return *str? 1 : 0;
    }
  } else {
    return
        nonTrivial(rhs)?-1:0;
  }
}

bool Zstring::operator ==(const char *rhs) const
{
  return same(str,rhs);
}

char *Zstring::cstr() const
{
  return str;
}

void Zstring::free()
{
  ::free(str);
  str=nullptr;
}
