#ifndef PATHPARSER_H
#define PATHPARSER_H

#include "textpointer.h"
#include "segmentedname.h"

class PathParser {
  char seperator;
public:
  PathParser();

  static Text pack(const SegmentedName &pieces,char seperator);
  /** destructive parser, if that bothers you copy construct the packed arg.
   * EG: this will segfault if you try to test it with a string constant (although you have to work hard to prevent a copy from occuring before you get here).
   *  @returns whether input was at root (stars with seperator */
  static bool parseInto(SegmentedName &pieces, Text &packed, char seperator);

  static Text makeNumber(double value);
}; // class PathParser

#endif // PATHPARSER_H
