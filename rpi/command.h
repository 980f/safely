#ifndef COMMAND_H
#define COMMAND_H

#include "charformatter.h"
/** stx  comma seperated decimals checksum etx*/
class Command {
  u8 raw[512];//overkill
  double params[15];//overkill
public:
  ByteScanner receiver;

  Command();
  bool commandPresent();
  /** args are valid from when parse() returns true until next etx is encountered.
   * wise use would rewind the args as soon as they have been reacted to.
   * The 'used' args are the command. @see takeArgs for best use.
 */
  ArgSet args;//exposed for easy peeking
  /** @returns a wrapper around the args that are present, and conditionally marks then as used*/
  ArgSet takeArgs(bool rewind=true);

protected:
  bool validCommand();
};

#endif // COMMAND_H
