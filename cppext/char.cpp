#include "char.h"
#include "ctype.h"
//#include "cheaptricks.h"

bool isPresent(const char *flags, char flag){
  int badStringLimiter = 26; //in case string pointer is garbage we don't want to read all of ram

  if(flags) {
    char probe;
    while((probe = *flags++) && badStringLimiter-- > 0) {
      if(flag == probe) {
        return true;
      }
    }
  }
  return false;
} /* isPresent */

////////////////////////////////////

bool Char::numAlpha() const noexcept {
  return isalnum(raw) || isPresent("+-.", raw);
}

bool Char::startsName() const noexcept {
  return isalpha(raw);
}

bool Char::isDigit() const noexcept{
  return isdigit(raw);
}

bool Char::isControl() const noexcept{
  return iscntrl(raw);
}

bool Char::isInNumber() const noexcept {
  return isdigit(raw) || in("+-.Ee");
}

bool Char::isWhite() const noexcept {
  return isspace(raw);
}

bool Char::in(const char *tokens) const noexcept {
  return isPresent(tokens, raw);
}

bool Char::isHexDigit() const noexcept {
  return isxdigit(raw);
}

#include "ignoresignwarnings.h"
unsigned Char::hexDigit() const noexcept {
  unsigned trusting=(raw|0x20) - '0';//tolowerthen subtract char for zero.
  if((trusting>9)){
    trusting-=39;
  }
  return trusting; //'A'-'0' = 17, want 10 for that
}

char Char::hexNibble(unsigned sb) const noexcept {
  unsigned char nib= 15&(raw>>(sb*4)); //push to low nib
  return nib>9? 'A'+nib-10: '0'+nib;
}
