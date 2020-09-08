#pragma once

#include "buffer.h"
#include "halfopen.h" //includes Span
//todo: restore? #include "converter.h"
/** 
chop up a string incrementally, returns "string slice" for each field.
This is extracted from the batch mode version DottedName/SegmentedName/PathParser.

*/

class PathIterator {
public:
  /** path description/state */
  struct Rules {
    /** what to put between items as they are concatenated */
    char slash; //maydo: unicode char, after the basics are well tested
    /** whether to add a trailing slash */
    bool after;
    /** whether to start with the slash */
    bool before;
    /** whether immediately succesive slashes get treated as a single (//=>/)*/
    bool coalesce;
    /** using most frequent by runtime instances settings as defaults */
    Rules(char slash='/', bool after = false,bool before = false,bool coalesce=true);
    bool isSlash(const char token){
      return token==slash;
    }
  };

//  struct FullRules :public Rules {
//    Converter &converter;
//    FullRules();
//  };

public:
  Rules bracket;
  PathIterator(char seperator);//initializes bracket member
  PathIterator(Rules bracket);
  /** called when receiver is empty, to detect quantity of leading slashes, our exception to the rule of treating multiples as a single (for http and the like) */
  unsigned start(Indexer<const char> &scan);

  /** @returns indexes identifying substring, excluding seperator. the returned Span will be empty() when the packed buffer is exhausted.
   * the buffer ordinal may be used to identify leading slash, although attempts have been made to be smarter than that. */
  Span next(Indexer<const char> &packed);

};


