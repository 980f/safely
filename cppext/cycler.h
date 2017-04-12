#ifndef CYCLER_H
#define CYCLER_H

/** modular counter */
class Cycler {
  unsigned length;
  unsigned value;
  /** fixup c's inconvenient definition of modulo*/
  void unwrap();
public:
  Cycler(int length);
  void setLength(unsigned length);
  /** @returns teh cycle length */
  unsigned cycle()const;
  /** @returns present phase in the cycle */
  operator unsigned(void)const;

  /** @returns true once per cycle, and not until the end of the first cycle if used in a typical fashion*/
  operator bool(void);
  /** modulo */
  unsigned operator +(int offset)const;
  unsigned operator +=(int offset);

  unsigned operator -(int offset)const;
  unsigned operator = (int force);
  unsigned increment(void);
  unsigned operator++ (void); ///pre increment
  unsigned operator++ (int dummy); ///post increment
};

#endif // CYCLER_H
