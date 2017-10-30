#ifndef FILENAMECONVERTER_H
#define FILENAMECONVERTER_H

#include "converter.h"
#include "buffer.h"
class FileNameConverter:public Converter {
  //for now do nothing, need to get a compile.
  //later on convert filesystem chars into %xx escapes.
  /** @returns length required for converted @param source */
  unsigned length(const char * source) const override;
  /** convert @param source into @param packer. Will truncate if you do not make packer big enough to contain what @see length() returns for the @param source */
  bool operator()(const char * source,Indexer<char> &packer) override;

};


#endif // FILENAMECONVERTER_H
