#pragma once // "(C) Andrew L. Heilveil, 2017"

/** like strchr but with idiot checks on the parameters */
bool isPresent(const char *flags, char flag);

class Char {
public:
  char raw;

public:
  Char(const char raw = 0) { // NOLINT(*-explicit-constructor)
    this->raw = raw;
  }

  Char &operator =(const char raw) {
    this->raw = raw;
    return *this;
  }

  /** compare byte. casts the incoming int to compare, a -1 parameter will match a 0xFF raw value.
   * @returns whether this is the same value as @param ch . */
  bool is(int ch) const noexcept {
    return raw == static_cast<char>(ch);
  }

  /** @returns @see is() */
  bool operator ==(int ch) const noexcept {
    return is(ch);
  }

  operator char() const noexcept { //cuts down on compiler complaints, except in switches.
    return raw;
  }

  /** @returns whether this is allowed in numeric constant or enum name */
  bool numAlpha() const noexcept;

  /** @returns whether this is first char of an identifier, per JSON and C++ rules. This is pretty much anything that isn't a digit, punctuation, or a control char */
  bool startsName() const noexcept;

  /** @returns whether this is a decimal digit */
  bool isDigit() const noexcept;

  bool isLower() const noexcept;

  char asLower() const noexcept;

  bool isUpper() const noexcept;

  char asUpper() const noexcept;

  bool isControl() const noexcept;

  /** @returns whether this is first char of a number image, per JSON and C++ rules */
  bool startsNumber() const noexcept;

  /** @returns whether this is non-initial char of a number. Doesn't retain state so defective numbers will still get a true here*/
  bool isInNumber() const noexcept;

  /** @returns whether this is considered whitespace */
  bool isWhite() const noexcept;

  /** @returns whether this object's char value is in the string @param tokens */
  bool in(const char *tokens) const noexcept;

  /** @returns whether this is a legal hex digit per C's rules */
  bool isHexDigit() const noexcept;

  /** @returns the math value of this which is presumed to be a hexdigit, wild (but repeatable) trash if not.*/
  unsigned hexDigit() const noexcept;

  /** append this as hex digit to @param uch such as a utf8 to Unicode assembler. Presumes caller has already checked isHexDigit() */
  template<typename Intish> void hexDigit(Intish &uch) const noexcept {
    uch <<= 4;
    uch |= hexDigit();
  }

  /** if this is a hexadecimal digit then apply it to @param accumulator, @returns whether it did so */
  template<typename Intish> bool appliedNibble(Intish &accumulator) const noexcept {
    if (isHexDigit()) {
      hexDigit(accumulator);
      return true;
    } else {
      return false;
    }
  }

  /** when you are really sure that this is a decimal digit then call this guy, else call @see appliedDigit */
  template<typename Intish> void applyTo(Intish &accumulator) const noexcept {
    unsigned digit = raw - '0';
    accumulator *= 10;
    accumulator += digit;
  }

  /** if this is a decimal digit then apply it to @param accumulator, @returns whether it did so */
  template<typename Intish> bool appliedDigit(Intish &accumulator) const noexcept {
    if (isDigit()) {
      applyTo(accumulator);
      return true;
    } else {
      return false;
    }
  }

  /** hex digit character for the @param sb th digit, 0==low or 1=high nibble of this char.
   * we could use a boolean for the nibble select, but want this to look like the unicode variation of this class.*/
  char hexNibble(unsigned sb) const noexcept;

  /** @returns whether this char needs a slash preceding it */
  bool needsSlash() const noexcept;

  /** @returns the c-escape partner of this. 'n' goes to newline, a newline goes to 'n' */
  char slashee() const noexcept;
};
