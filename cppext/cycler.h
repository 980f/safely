#ifndef CYCLER_H
#define CYCLER_H

/** modular counter */
class Cycler {
  int length;
  int value;
public:
  Cycler(int length);
  void setLength(int length);
  int cycle();
  operator unsigned(void);
  operator int(void);
  /** @returns true once per cycle, and not until the end of the first cycle if used in a typical fashion*/
  operator bool(void);
  int operator +(int offset);
  int operator +=(int offset);

  int operator -(int offset);
  int operator = (int force);
  int increment(void);
  int operator++ (void); ///pre increment
  int operator++ (int dummy); ///post increment
};

#endif // CYCLER_H
