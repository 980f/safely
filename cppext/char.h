#ifndef CHAR_H
#define CHAR_H


class Char {
public:
  char raw;

public:
  Char(char raw = 0){
    this->raw = raw;
  }

  Char&operator =(char raw){
    this->raw = raw;
    return *this;
  }

  /** compare byte.
   * @returns whether this is the same value as @param ch. */
  bool is(int ch) const noexcept {
    return raw == char(ch);
  }

  /** @returns @see is() */
  bool operator ==(int ch) const noexcept {
    return is(ch);
  }

  operator char(void) const  noexcept{ //cuts down on compiler complaints, except in switches.
    return raw;
  }

  /** @returns whether this is allowed in numeric constant or enum name */
  bool numAlpha() const noexcept;

  /** @returns whether this is first char of an identifier, per JSON and C++ rules. This is pretty much anything that isn't a number, punctuation or a control char */
  bool startsName() const noexcept;

  /** @returns whether this is a decimal digit */
  bool isDigit() const noexcept;

  bool isControl() const noexcept;
  /** @returns whether this is first char of an number image, per JSON and C++ rules */
  bool startsNumber() const noexcept;

  /** @returns whether this is non-initial char of a number. Doesn't retain state so defective numbers will still get a true here*/
  bool isInNumber() const noexcept;

  /** @returns whether this is considered whitespace */
  bool isWhite() const noexcept;

  bool in(const char *tokens) const noexcept;
  /** @returns whether this is a legal hex digit per C's rules */
  bool isHexDigit() const noexcept;
  /** @returns the math value of this which is presumed to be a hexdigit, wild (but repeatable) trash if not.*/
  unsigned hexDigit() const noexcept;

  char hexNibble(unsigned sb) const noexcept;
};

#endif // CHAR_H
