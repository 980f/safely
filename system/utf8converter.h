#ifndef UTF8CONVERTER_H
#define UTF8CONVERTER_H
/** mate utf8 logic to Indexer<char>
*/
#include "converter.h"
#include "utf8.h"
#include "utf8transcoder.h"

/** utf8 to \ u etc. */
class Utf8ConverterOut: public Converter {
public:
  virtual ~Utf8ConverterOut()=default;
  /** length required by conversion of source */
  unsigned length(const char * source) const override ;
  /** convert source into @param packer */
  void operator()(const char * source,Indexer<char> &packer) override ;
};

/** escape to utf8 */
class Utf8ConverterIn: public Converter {
public:
  virtual ~Utf8ConverterIn()=default;
  /** length required by conversion of source */
  unsigned length(const char * source) const override;
  /** convert source into @param packer */
  void operator()(const char * source,Indexer<char> &packer) override ;
};

class UnicharScanner:public ReadonlySequence<Unichar> {
  Indexer<char> utf8;
  Utf8Escaper ex;
public:
  virtual ~UnicharScanner()=default;
  /** for @param rewind see Indexer class, ~0 if accessing already used, 0 for the whole thing 1 for backup and then remainder of @param utf8 */
  UnicharScanner(const Indexer<char>&utf8, int rewind = 0);
  bool hasNext(void) const override;
  Unichar next(void) override;
};

#endif // UTF8CONVERTER_H
