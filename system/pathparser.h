#ifndef PATHPARSER_H
#define PATHPARSER_H

#include "textpointer.h"
#include "segmentedname.h"
#include "converter.h"

namespace PathParser {

/** path description/state */
struct Rules {
  /** what to put between items as they are concatenated */
  char slash; //maydo: unicode char, after the basics are well tested
  /** whether to add a trailing slash */
  bool after;
  /** whether to start with the slash */
  bool before;
  /** using most frequent by runtime instances settings as defaults */
  Rules(char slash='/', bool after = false,bool before = false);
};


/** append the pieces together separated by seperator and conditionally wrapped with seperators.
 * the lead and trailing seperators are only added when requested and when they would not result in a lonely slash
 * IE an empty pathname does NOT become '/'. That is an attempt to preclude 'rm -rf /' */
Text pack(const SegmentedName &pieces, const Rules &rule=Rules(), Converter &&converter=Converter());

unsigned length(ConstChainScanner<Cstr> indexer, const Rules &rule=Rules(), Converter &&converter=Converter());

/** @returns whether input was at root (starts with seperator and whether there was a trailing one as well */
Rules parseInto(SegmentedName &pieces, const Text &packed, char seperator);


void packInto(Indexer<char> &packer, ConstChainScanner<Cstr> indexer, const Rules &rule=Rules(), Converter &&converter=Converter());

}  // class PathParser

#endif // PATHPARSER_H
