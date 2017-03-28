#ifndef DOTTEDNAME_H
#define DOTTEDNAME_H

#include "pathparser.h"

/** class wrapper around pathparser common use*/
/** deriving for syntactic convenience. */
class DottedName:public SegmentedName {
public://for access to before, might make an accessor for just that.
  PathParser::Rules bracket;
public:
  DottedName(char dot,Cstr initial);
  DottedName(char dot,Indexer<const char> initial);
  /** */
  void parse(const char *rawpath);
  void parse(Indexer<const char> scan);

  /** @returns bytes needed for image of this.  @param cvt is used to encode the bytes (convert into escape sequences as needed) */
  unsigned length(Converter &&cvt) const;
  /** makes and @returns an image of this */
  Text pack(Converter &&cvt);

};
#endif // DOTTEDNAME_H
