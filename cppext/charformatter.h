#ifndef CHARFORMATTER_H
#define CHARFORMATTER_H
#include "charscanner.h"
#include "argset.h"
/** print human readable stuff into a checked buffer.
 *  next layer down tries to preserve a terminating null, but you have to ask it to do so.
 */

class CharFormatter : public CharScanner {
public:
  /** unsafe version, uses strlen */
  CharFormatter(char * content);
  /** @parm size as given must be the size allocated with @param content */
  CharFormatter(char * content, int size);
  /** construct around buffer associated with @param other, shares the buffer!*/
  CharFormatter(CharScanner &other);
  /** construct around buffer associated with @param other, shares the buffer!*/
  CharFormatter(ByteScanner &other);

  /** parse a double from current pointer */
  double parseDouble(void);

  s64 parse64(s64 def = 0);

  int parseInt(int def = -1);

  /** all of the print... functions make sure that the number doesn't get truncated,
   *  and return false if it would have been.*/
  bool printChar(char ch);
  bool printChar(char ch, unsigned howMany);
  /** sensible results for digit 0 through 15, potentially garbage if digit>15.*/
  bool printDigit(unsigned digit);
  bool printUnsigned(unsigned int value);
  bool printSigned(int value);
  bool printNumber(double d, int sigfig = 9);//9: 200 million is biggest number that we won't lose a bit of.

  bool printString(const char *s);

  void printArgs(ArgSet&args,bool master);
  /** useful for collation sequence, -1: this before other, +1: this after other, 0: this same as other*/
  int cmp(const CharScanner&other) const;

  bool addDigest();
  bool addTerminator();
  bool removeTerminator();
  /** Prints decimal representation of @param value, prepending it with spaces until the total length == @param width.
   *@returns false if there wasn't enough space. or if the number of digits is larger than the specified width in which latter case this function will fill the field
   *with asterisks */
  bool printAtWidth(unsigned int value, unsigned width);
  /** print @param width least significant hex digits of @param value, @returns whether there were width positions available, if not then nothing is written */
  bool printHex(unsigned value, unsigned width);
}; // class CharFormatter


#endif // CHARFORMATTER_H
