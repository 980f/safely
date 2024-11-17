#pragma once

/** modular counter. its integral value is always unsigned and between 0 and length-1
 * If length is zero then the value is ~0 regardless of operations applied to it.
 *
 * originally the length was variable, now it is a construction time constant for performance reasons.
 * Flying changes of length can be implemented via opening a block and creating one with that block.
 *
 * If you set the length to zero expect the index/value to always be ~0 regardless of how you manipulate it.
 *
 * A templated version might be slightly faster, the length value would be fetched from the instruction stream rather than an actual memory cycle, but the use cases so far are too few to bother dealing with how to name them.
 *
 */
class Cycler {
  const unsigned length;
  unsigned value;

  /** fixup c's inconvenient definition of modulo */
  constexpr unsigned unwrap(int value) const;

public:
  Cycler(unsigned length);

  /** @returns whether @param index is within the defined cycle*/
  constexpr bool contains(unsigned index) const;

  /** @returns the cycle length */
  constexpr unsigned Cycler::cycle() const {
    return length;
  }

  /** @returns present phase in the cycle */
  constexpr operator unsigned() const {
    return value;
  }

  //compiler preferred operator bool over operator unsigned for implicit conversions, it should have complained but instead we just don't have an  operator bool(void);

  unsigned operator =(int force);

  /** @returns true once per cycle, and not until the end of the first cycle if used in a typical fashion */
  bool next();

  /** force to start, IE value == 0 */
  void reset() {
    value = length ? 0 : ~0;
  }

  /** force value such that the subsequent next() yields true */
  void preset() {
    value=length?length-1:~0;
  }

  unsigned operator +=(int offset);

  unsigned increment();
  unsigned decrement();

  unsigned operator++(); ///pre increment
  unsigned operator++(int dummy); ///post increment
  unsigned operator--(); ///pre decrement
  unsigned operator--(int dummy); ///post decrement

/** for lookahead or lookbehind */
  constexpr unsigned operator +(int offset) const;
  constexpr unsigned operator -(int offset) const;

};
