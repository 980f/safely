#ifndef PATHPARSER_H
#define PATHPARSER_H

#include "textpointer.h"
#include "segmentedname.h"

class PathParser {

public:
  PathParser();
  struct Brackets {
    bool after=false;
    bool before=false;
    Brackets(bool after=false,bool before=false);
  };

  /** the pack() functions append the pieces together separated by seperator and conditionally wrapped with seperators.
   * the lead and trailing seperators are only added when requested and when they would not result in a lonely slash
   * IE an empty pathname does NOT become '/'. That is an attempt to preclude 'rm -rf /' */
  static Text pack(const SegmentedName &pieces, char seperator, Brackets bracket);
  /** @param after is whether to add a trailing seperator, such as to indiciate 'directory' to some shell commands,
   * @param before is whether to prefix path with seperator, such as for an absolute disk path. */
  static Text pack(const SegmentedName &pieces, char seperator, bool after=false, bool before=false);
  /** @returns whether input was at root (starts with seperator and whether there was a trailing one as well */
  static Brackets parseInto(SegmentedName &pieces, Text &packed, char seperator);

}; // class PathParser

#endif // PATHPARSER_H
