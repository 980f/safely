#ifndef CONVERTER_H
#define CONVERTER_H

#include "buffer.h"

/** interface for encoder/decoders. base implementation does no noop conversion*/
class Converter {
public:
  virtual ~Converter();
  /** length required by conversion of source */
  virtual unsigned length(const char * source) const;
  /** convert source into @param packer */
  virtual void operator()(const char * source,Indexer<char> &packer);
};

#endif // CONVERTER_H
