#ifndef PATHPARSER_H
#define PATHPARSER_H

#include "textpointer.h"
#include "segmentedname.h"
#include "converter.h"

namespace PathParser {

/** the seperator was always known at the same place the other flags were known so I am binding them all together.
 * */
struct Rules {
  /** what to put between items as they are concatenated */
  char slash; //maydo: unicode char, after the basics are well tested
  /** whether to add a trailing slash */
  bool after;
  /** whether to start with the slash */
  bool before;
  /** using most frequent settings as defaults */
  Rules(char slash='/', bool after = false,bool before = false);
};


/** the pack() functions append the pieces together separated by seperator and conditionally wrapped with seperators.
 * the lead and trailing seperators are only added when requested and when they would not result in a lonely slash
 * IE an empty pathname does NOT become '/'. That is an attempt to preclude 'rm -rf /' */
Text pack(const SegmentedName &pieces, const Rules &rule=Rules(), Converter &&converter=Converter());
///** pack without leading or trailing slashes */
//Text pack(const SegmentedName &pieces);

//Text pack(const SegmentedName &pieces, char seperator, bool after = false, bool before = false);

/** @returns whether input was at root (starts with seperator and whether there was a trailing one as well */
Rules parseInto(SegmentedName &pieces, const Text &packed, char seperator);

}  // class PathParser

#endif // PATHPARSER_H
