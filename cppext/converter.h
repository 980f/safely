#pragma once // "(C) Andrew L. Heilveil, 2017"

#include "buffer.h"

/** interface for encoder/decoders. base implementation does a noop conversion.
 * This is part of a scheme of parsimonious malloc'ing, minimal calls to malloc and for buffers only as big as needed.
 */
class Converter {
public:
  virtual ~Converter() = default;

  /** @returns length required for converted @param source */
  virtual unsigned length(const char *source) const;

  /** convert @param source into @param packer. Will truncate if you do not make packer big enough to contain what @see length() returns for the @param source.
 @returns whether it did NOT truncate */
  virtual bool operator()(const char *source, Indexer<char> &packer);

  /** quite a few instances of passing a converter around by reference needed an r-value reference.
   * This guy does the equivalent of std::forward<Converter>(converterarg) with less painful typing */
  Converter &&forward() {
    return static_cast<Converter &&>(*this);
  }
};
