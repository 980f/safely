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
  bool isSlash(const char token){
    return token==slash;
  }
};

struct FullRules :public Rules {
  Converter &converter;
  FullRules();
};

/** append the pieces together separated by seperator and conditionally wrapped with seperators.
 * the lead and trailing seperators are only added when requested and when they would not result in a lonely slash
 * IE an empty pathname does NOT become '/'. That is an attempt to preclude 'rm -rf /' */
Text pack(const SegmentedName &pieces, const Rules &rule=Rules(), Converter &&converter=Converter());

/** @returns the size of a buffer that will be needed to pack the path implied by @param indexer */
unsigned length(ConstChainScanner<Text> indexer, const Rules &rule=Rules(), Converter &&converter=Converter());
/** build a packed image in the @param given packer buffer, you must allocate that buffer via @see length() */
void packInto(Indexer<char> &packer, ConstChainScanner<Text> indexer, const Rules &rule=Rules(), Converter &&converter=Converter());


/** @returns whether input was at root (starts with seperator and whether there was a trailing one as well.
 no converter is passed as it is more efficient to apply such to the data before parsing, and there might be converters for both before and after parsing */
Rules parseInto(SegmentedName &pieces, const Text &packed, char seperator);
Rules parseInto(SegmentedName &pieces, Indexer<const char> &scan, char seperator);

/** incremental version of parseInto, you will have to put the returned into something like a Chain<Cstr>.
 this was extracted from parseInto to deal with filenames having two seperators */
struct Chunker {
  Rules bracket;
  Chunker(char seperator);//initializes bracket member
  /** called when receiver is empty, to detect quantity of leading slashes, our exception to the rule of treating multiples as a single (for http and the like) */
  unsigned start(Indexer<const char> &scan);

  /** @returns indexes identifying substring, excluding seperator. the returned Span will be empty() when the packed buffer is exhausted.
   * the buffer ordinal may be used to identify leading slash, although attempts have been made to be smarter than that. */
  Span next(Indexer<const char> &packed);

  /** @see PathParser::parseInto. this is a lower performance version used to module test @see Chunker. */
  static Rules parseInto(SegmentedName &pieces, const Text &packed, char seperator);

};

}  // class PathParser

#endif // PATHPARSER_H
