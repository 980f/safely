#ifndef PATHPARSER_H
#define PATHPARSER_H

#include "buffer.h"
#include "halfopen.h" //includes Span
#include "converter.h"
/** return successive spans (slices in some languages) of a string without modifying it.

@deprecated looks broken in next() where it modifies the rule instead of checking it. Seems to be an untested extract of different type of parsing.
*/

class PathIterator {
  /** path description/state */
  struct Rules {
    /** what is between concatenated items */
    char slash; //maydo: unicode char, after the basics are well tested
    /** whether to report empty field after a trailing slash */
    bool after;
    /** whether to report an initial field that does not have a slash */
    bool before;
    /** whether immediately succesive slashes get treated as a single (//=>/)*/
    bool coalesce;
    /** using most frequent by runtime instances settings as defaults */
    Rules(char slash='/', bool after = false,bool before = false,bool coalesce=true);
    bool isSlash(const char token){
      return token==slash;
    }
  };

  struct FullRules :public Rules {
    Converter &converter;
    FullRules();
  };

public:
  Rules bracket;
  PathIterator(char seperator);//initializes bracket member
  /** called when receiver is empty, to detect quantity of leading slashes, our exception to the rule of treating multiples as a single (for http and the like) */
  unsigned start(Indexer<const char> &scan);

  /** @returns indexes identifying substring, excluding seperator. the returned Span will be empty() when the packed buffer is exhausted.
   * the buffer ordinal may be used to identify leading slash, although attempts have been made to be smarter than that. */
  Span next(Indexer<const char> &packed);

};

#endif // PATHPARSER_H
