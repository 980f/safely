#include "char.h"
#include "ctype.h"
#include "cheaptricks.h"

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
  unsigned trusting=(raw &~0x20) - '0';//toUpper then subtract char for zero.
  return (trusting>9)? trusting-7: trusting; //'A'-'0' = 17, want 10 for that
}

char Char::hexNibble(unsigned sb) const noexcept {
  u8 nib= 15&(raw>>(sb*4)); //push to low nib
  return nib>9? 'A'+nib-10: '0'+nib;
}
