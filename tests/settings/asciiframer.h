#ifndef ASCIIFRAMER_H
#define ASCIIFRAMER_H
#include "eztypes.h"
#include "fifo.h"
#include "charscanner.h"
#include "asciiframing.h"
#include "serialagent.h"
/**
  * command line editor
  * ^C erases all input, including buffered but not inspected stuff.
  * \n is "line complete"
  *
  */
class AsciiFramer:public SerialAgent{
  Fifo fifo;
  bool cred;
  bool purgeUntilNewline;
public:
  AsciiFramer(char * storage, int sizeofstorage);
  int eols; //functions as counted boolean
  /**while public flush should not be called while interrupts are enabled. see burn*/
  void flush();
  /** @return whether this can receive more chars*/
  bool onReception(int octet) ISRISH;

  /** @return whether line was present and copied over
    * processes ascii control chars writing cooked ones to given scanner.
    * deslash is not yet implemented but will enable processing of the standard c escape strings, which isn't needed unless we pass strings to the QCU for display.
    */
  bool getLine(CharScanner&p, bool deslash = false);
  /** @return whether the whole sting was stuffed.
    * presently doesn't undo partially adding the string, might change that in the future.*/
  bool stuff(const char *preloads);
  /** start of frame symbol, could #define but this should generate the same code if the compiler's optimizer is decent:*/

};

#endif // ASCIIFRAMER_H
