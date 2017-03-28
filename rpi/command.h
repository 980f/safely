#ifndef COMMAND_H
#define COMMAND_H

#include "charformatter.h"
#include "dottedname.h" //comma separated list
/** stx  comma seperated decimals checksum etx*/
class Command {
  char raw[512];//overkill
public:
  /** an array of text fields */
  DottedName parsed;
  /** comma separated string */
  CharScanner receiver;

  Command();
  bool commandPresent();
  /** args are valid from when parse() returns true until next etx is encountered.
   * wise use would rewind the args as soon as they have been reacted to.
   * The 'used' args are the command. @see takeArgs for best use.
 */
  static char checksum(CharScanner p);

  /** when actually a response, call this to add a checksum to the fields in parsed then pack them into the receiver */
  void packsum();
protected:
  bool validCommand();
};

#endif // COMMAND_H
