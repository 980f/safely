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
  /** */
  void parse(const char *rawpath);
  /** @returns bytes needed for image of this*/
  unsigned length(Converter &&cvt) const;
  /** makes and @returns an image of this */
  Text pack(Converter &&cvt);

};
#endif // DOTTEDNAME_H
